#!/usr/bin/env python3

from pathlib import Path

OUTDIR = Path.cwd() / "wheelhouse"


import argparse
import subprocess


def main(args):
    subprocess.run(["delocate-wheel", "--dylibs-only",
                    "--wheel-dir", OUTDIR,
                    args.wheel
                    ],
                   check=True
                   )

def script():
    p = argparse.ArgumentParser(description="Vendor in .dylib dependencies of a wheel using delocate.")
    p.add_argument("--wheel", required=True, metavar="<path>")
    args = p.parse_args()
    main(args)


if __name__ == '__main__':
    script()
