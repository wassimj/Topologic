#!/bin/bash
set -e

function getmacosversion {
    local IFS='.'
    macosversion=( $(sw_vers -productVersion) )
}
getmacosversion

# Solved with the help of https://github.com/pypa/wheel/issues/406
#   "arm64-only package selects universal2 as the arch tag".
#
# 'universal2' tag breaks 'repair_wheel_macos.py' run.
# On macOS, .wht file must be platform specific, so tell setup.py to perform
# platform specific build by setting CMAKE_OSX_ARCHITECTURES and
# TOPOLOGIC_PLAT_NAME.

# Example of MACOSX_DEPLOYMENT_TARGET: 13.0
export MACOSX_DEPLOYMENT_TARGET=${macosversion[0]}.0
export TOPOLOGIC_EXTRA_CMAKE_ARGS="-DCMAKE_OSX_ARCHITECTURES=arm64"
# Example of TOPOLOGIC_PLAT_NAME: macosx_13_0_arm64
export TOPOLOGIC_PLAT_NAME=macosx_${MACOSX_DEPLOYMENT_TARGET//./_}_arm64

python -m platform
python --version

# Obtain project root/top directory and stay in it.
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"/..
TOP_DIR=$(pwd)

# Install tools and dependencies.
brew install cmake ninja opencascade

# Show tools.
ninja --version
cmake --version

# Add Python tool(s) required by 'repair_wheel_macos.py'.
pip install delocate

# Parse Python version in format for the project.
PYTHONVER=`python -c 'import sys; print(f"{sys.version_info[0]}{sys.version_info[1]}")'`
echo "Parsed PYTHONVER is ${PYTHONVER}"

# Build the project.
cd TopologicPythonBindings
python build_macos.py

# Run the tests.
pip install wheelhouse/topologic-5.0.0-cp${PYTHONVER}-cp${PYTHONVER}-${TOPOLOGIC_PLAT_NAME}.whl
cd test
python topologictest01.py
python topologictest02.py

echo "Python bindings have been built successfully."
