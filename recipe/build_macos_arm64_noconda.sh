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
# On macOS, .whl file must be platform specific, so tell setup.py to perform
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

cd TopologicPythonBindings

# By using TOPOLOGIC_OUTPUT_ID environment variable, it's possible to collect
# variables printed by setup.py to file "${TOPOLOGIC_OUTPUT_ID}.log".
export TOPOLOGIC_OUTPUT_ID=${RANDOM}${RANDOM}${RANDOM}
TOPOLOGIC_OUTPUT_FILE_PATH=${PWD}/${TOPOLOGIC_OUTPUT_ID}.log
trap '{ rm -f -- "$TOPOLOGIC_OUTPUT_FILE_PATH"; }' EXIT

# Build the project.
python build_macos.py

# Obtain wheel name, e.g. "topologic-5.0.0-cp312-cp312-linux_x86_64.whl"
WHEEL_NAME=$(sed -n 's/^WHEEL_NAME=//p' "$TOPOLOGIC_OUTPUT_FILE_PATH")

# Run the tests.
pip install "wheelhouse/${WHEEL_NAME}"
cd test
python topologictest01.py
python topologictest02.py

echo "Python bindings have been built successfully."
