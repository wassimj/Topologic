
# originally based on https://github.com/vinayak-mehta/pdftopng/blob/6e4a9d589228f66dfbb6f0197740b0b7c1bef9b3/scripts/wheel_repair.py
# https://vinayak.io/2020/10/22/day-52-bundling-dlls-with-windows-wheels-the-dll-mangling-way/

from pathlib import Path

# excluding api-ms-crt from mangling (found in conda envs)
EXCLUDES = [r"^python3.*\.dll", r"^api-ms-win-crt-.*\.dll"]
OUTDIR = Path.cwd() / "wheelhouse"  # auditwheel default


import os
import argparse
import re
import shutil
import tempfile
import zipfile
import hashlib
from collections import OrderedDict
from itertools import islice

import pefile
from machomachomangler.pe import redll


def hash_filename(filepath):
    hasher = hashlib.sha256()
    bufsize = 64*1024

    with open(filepath, "rb") as f:
        buf = f.read(bufsize)
        while len(buf) > 0:
            hasher.update(buf)
            buf = f.read(bufsize)

    root, ext = os.path.splitext(filepath)
    return f"{os.path.basename(root)}-{hasher.hexdigest()[:8]}{ext}"


def mangle_dll(infile, outfile, mapping):
    with open(infile, "rb") as f:
        buf = f.read()

    new_buf = redll(buf, mapping)

    with open(outfile, "wb") as f:
        f.write(new_buf)


def find_dll_dependencies(dll_filepath, bin_dir, excludes=None):
    dll_dependencies = OrderedDict()

    def _find(dll_filepath, bin_dir):
        dll_basename = os.path.basename(dll_filepath)
        dll_dependencies.setdefault(dll_basename, set())
        pe = pefile.PE(dll_filepath)
        for entry in getattr(pe, "DIRECTORY_ENTRY_IMPORT", []):
            entry_basename = entry.dll.decode("utf-8")
            if entry_basename not in os.listdir(bin_dir): continue
            if excludes:
                m = None
                for pat in excludes:
                    if m := re.match(pat, entry_basename):
                        break
                if m: continue
            dll_dependencies[dll_basename].add(entry_basename)
            if entry_basename not in dll_dependencies:
                _dll_filepath = os.path.join(bin_dir, entry_basename)
                _find(_dll_filepath, bin_dir)
            else:
                continue
        pe.close()

    _find(dll_filepath, bin_dir)
    return dll_dependencies


def main(args):
    global EXCLUDES, OUTDIR

    wheel_name = os.path.basename(args.WHEEL)
    package_name = wheel_name.split("-")[0]
    repaired_wheel = OUTDIR / wheel_name

    OUTDIR.mkdir(exist_ok=True)
    old_wheel_dir = tempfile.mkdtemp(dir=OUTDIR)
    new_wheel_dir = tempfile.mkdtemp(dir=OUTDIR)

    with zipfile.ZipFile(args.WHEEL, "r") as wheel:
        wheel.extractall(old_wheel_dir)
        wheel.extractall(new_wheel_dir)
        wheel_pyds = [x for x in wheel.namelist() if x.endswith(".pyd")]

    pyd_basename = os.path.basename(wheel_pyds[0])
    pyd_tmp_path = os.path.join(old_wheel_dir, package_name, os.path.basename(pyd_basename))
    if len(wheel_pyds) > 1:
        raise NotImplemented("can't process more than on .pyd")
    if len(wheel_pyds) == 0:
        raise ValueError(f"no .pyd in wheel: {args.WHEEL}")

    # deps mapping
    # first item is our .pyd
    map_deps = find_dll_dependencies(pyd_tmp_path, args.DLL_DIR, excludes=EXCLUDES)

    # hashed names mapping
    map_hashed_names = {}
    for dll_basename in islice(map_deps.keys(), 1, None):
        hashed_name = hash_filename(os.path.join(args.DLL_DIR, dll_basename))
        map_hashed_names[dll_basename] = hashed_name

    def mangle_or_copy(infile, outfile, mapping):
        if not mapping:
            print(f"copy {infile} -> {outfile}")
            shutil.copyfile(infile, outfile)
        else:
            print(f"mangle {infile} -> {outfile}")
            mapping_ascii = {k.encode("ascii"): v.encode("ascii") for k, v in mapping.items()}
            mangle_dll(infile, outfile, mapping_ascii)

    # mangle deps: each dll is copied or written out with its imports and own filename changed to hashed names
    for dll_basename, deps in islice(map_deps.items(), 1, None):
        old_name = os.path.join(args.DLL_DIR, dll_basename)
        hashed_name = map_hashed_names[dll_basename]
        new_name = os.path.join(new_wheel_dir, package_name, hashed_name)

        if os.path.exists(new_name): continue
        mapping_mangle = { dep_basename: map_hashed_names[dep_basename] for dep_basename in deps }
        mangle_or_copy(old_name, new_name, mapping_mangle)

    # mangle our .pyd
    old_name = pyd_tmp_path
    new_name = os.path.join(new_wheel_dir, package_name, os.path.basename(pyd_tmp_path))
    mapping_mangle = {dep_basename: map_hashed_names[dep_basename] for dep_basename in map_deps[pyd_basename]}
    mangle_or_copy(old_name, new_name, mapping_mangle)

    with zipfile.ZipFile(repaired_wheel, "w", zipfile.ZIP_DEFLATED) as new_wheel:
        for root, dirs, files in os.walk(new_wheel_dir):
            for f in files:
                new_wheel.write(os.path.join(root, f), os.path.join(os.path.basename(root), f))

    shutil.rmtree(old_wheel_dir)
    shutil.rmtree(new_wheel_dir)
    print(f"done: path to repaired wheel: {repaired_wheel}")


def script():
    p = argparse.ArgumentParser(description="Vendor in .dll dependencies of a wheel.")
    p.add_argument("--wheel", required=True, dest="WHEEL", metavar="<path>")
    p.add_argument("--dll-dir", required=True, dest="DLL_DIR", metavar="<path>")
    args = p.parse_args()
    main(args)


if __name__ == '__main__':
    script()
