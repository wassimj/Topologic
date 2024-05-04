import os
import subprocess
import shutil
import sys
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext
from setuptools.dist import Distribution


# taken from: https://github.com/pybind/cmake_example/blob/master/setup.py
# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = "") -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


# taken from: https://github.com/pybind/cmake_example/blob/master/setup.py
class CMakeBuild(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        # Must be in this form due to bug in .resolve() only fixed in Python 3.10+
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)  # type: ignore[no-untyped-call]
        extdir = ext_fullpath.parent.resolve()

        # Using this requires trailing slash for auto-detection & inclusion of
        # auxiliary "native" libs

        # our build code:

        build_type = "release" if not self.debug else "debug"

        source_dir = os.path.dirname(__file__) # dir containing setup.py
        # this setup.py is assumed to be a subproject of the main Topologic source tree
        source_dir = Path(source_dir) / ".."

        build_temp = Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        build_dir = build_temp / f"cmake-build-{build_type}"
        install_to = build_temp / f"cmake-install-{build_type}"

        build_target = "TopologicPythonBindings"
        install_component = build_target

        # build TopologicCore as static library to be linked into this extension
        extra_cmake_args = "-DTOPOLOGICCORE_BUILD_SHARED=OFF"

        # run build.py
        subprocess.run(
            [sys.executable, Path(source_dir) / "build.py",
             "--verbose",
             "--build-type", build_type,
             "--source-dir", source_dir,
             "--build-dir", build_dir, "--install-to", install_to,
             "--build-target", build_target, "--install-component", install_component,
             "--build-version", self.distribution.get_version(),
             "--extra-cmake-args", extra_cmake_args],
            check=True
        )

        # copy module from <install prefix>/lib/TopologicPythonBindings/
        ext_filename = os.path.basename(ext_fullpath)
        extdir.mkdir(parents=True, exist_ok=True)
        cp_src = install_to / "lib" / "TopologicPythonBindings" / ext_filename
        cp_dst = ext_fullpath
        self.announce(f"copying {cp_dst} -> {cp_dst}")
        shutil.copyfile(cp_src, cp_dst)

# For 'bdist_wheel', a custom platform name may be required.
# Apply and use the name if it is set in 'TOPOLOGIC_PLAT_NAME' env variable.
def build_options():
    if "TOPOLOGIC_PLAT_NAME" in os.environ:
        return {
            'bdist_wheel': {
                'universal': False,
                'plat_name': os.environ['TOPOLOGIC_PLAT_NAME']
            }
        }
    return {};

def try_read_actual_version(default_version):
    if "TOPOLOGIC_VERSION" in os.environ:
        return os.environ["TOPOLOGIC_VERSION"]
    return default_version

def print_wheel_name_to_output(**kwargs):
    if "TOPOLOGIC_OUTPUT_ID" not in os.environ:
        return
    output_name = os.environ["TOPOLOGIC_OUTPUT_ID"];
    if not output_name.isalnum():
        return
    output_name += '.log'

    dist = Distribution(attrs=kwargs)
    command_obj = dist.get_command_obj('bdist_wheel')
    command_obj.ensure_finalized()
    dist_name = command_obj.wheel_dist_name
    tag = '-'.join(command_obj.get_tag())
    f = open(output_name, "w")
    wheel_name = f"{dist_name}-{tag}.whl"
    print(f"Expected wheel name is {wheel_name} (WHEEL_NAME={wheel_name})");
    f.write(f'WHEEL_NAME={wheel_name}\n');
    f.close()

setup_kwargs = dict(
    name="topologic-core",
    version=try_read_actual_version(default_version="5.0.0"), # Syncs with: TopologicCore/CMakeLists.txt and TopologicCore/src/About.cpp
    author="Topologic Authors",
    author_email="None",
    description="TopologicPythonBindings wrapper package",
    long_description="",
    ext_modules=[CMakeExtension("topologic_core.topologic_core")],
    packages=["topologic_core"],
    package_dir={"topologic_core": 'python/topologic_core'},
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False, # install as dir
    extras_require={},
    python_requires=">=3.8",
    options=build_options()
);

print_wheel_name_to_output(**setup_kwargs)
setup(**setup_kwargs)
