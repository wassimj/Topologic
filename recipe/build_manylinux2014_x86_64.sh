#!/bin/bash
# This script is to be used in Docker of quay.io/pypa/manylinux2014_x86_64
# as recommended by 'manylinux' project.
set -exuo pipefail

# Obtain project root/top directory and stay in it.
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"/..
TOP_DIR=$(pwd)
OCCT_INSTALL_DIR=${TOP_DIR}/occt-installed-centos

# Parse optional arguments
vars=$(getopt -o on --long only-deps,no-deps -- "$@")
eval set -- "$vars"

NO_DEPS=0
ONLY_DEPS=0

# Extract options.
for opt; do
    case "$opt" in
      -o|--only-deps)
        ONLY_DEPS=1
        shift 1
        ;;
      -n|--no-deps)
        NO_DEPS=1
        shift 1
        ;;
    esac
done

## Install tools.
curl -sSL  -o ninja.zip "https://github.com/ninja-build/ninja/releases/download/v1.10.2/ninja-linux.zip"
unzip ninja.zip && mv ninja /usr/local/bin/ && rm -vf ninja* && ln -s /usr/local/bin/ninja /usr/local/bin/ninja-build
yum install -y cmake libuuid libuuid-devel

## Show tools.
ninja --version
cmake --version

if [[ "$NO_DEPS" -ne 1 ]]; then
  echo "Build OpenCASCADE 7.9.3 (takes long time)."
  if [ ! -d "${OCCT_INSTALL_DIR}" ]; then
    yum install -y \
          rapidjson-devel \
          tcllib tklib tcl-devel tk-devel libXtst \
          libXmu-devel mesa-libGL-devel tbb-devel \
          gl2ps-devel freetype-devel freeimage-devel

    git clone --depth 1 --branch V7_9_3 \
        https://github.com/Open-Cascade-SAS/OCCT.git \
        "${TOP_DIR}/occt-sources"

    cd "${TOP_DIR}/occt-sources"
    cmake -B build -S . \
          -D CMAKE_BUILD_TYPE="Release" \
          -D INSTALL_DIR=${OCCT_INSTALL_DIR} \
          -D BUILD_MODULE_Draw:BOOL=OFF \
          -D USE_TBB:BOOL=OFF \
          -D BUILD_RELEASE_DISABLE_EXCEPTIONS=OFF \
          -D USE_FREEIMAGE:BOOL=ON \
          -D USE_RAPIDJSON:BOOL=ON \
          -D BUILD_RELEASE_DISABLE_EXCEPTIONS:BOOL=OFF \
          -D 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2:FILEPATH="/usr/include/freetype2" \
          -D 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build:FILEPATH="/usr/include/freetype2"

    cd "${TOP_DIR}/occt-sources/build"
    make -j$(nproc --ignore=2)
    make install
    cd "${TOP_DIR}"
    # Create backup for openCASCADE headers and precompiled libs, just in case.
    rm -fr occt-installed-centos.tar.gz
    tar cvfz occt-installed-centos.tar.gz ./occt-installed-centos
  else
    echo "The directory '${OCCT_INSTALL_DIR}' for installed" \
         "openCASCADE exists. Remove the directory if you want to build" \
         "openCASCADE again (build takes long time)."
  fi

  if [[ "$ONLY_DEPS" -eq 1 ]]; then
    echo "Only dependencies. No need to build the project."
    exit 0
  fi
else
  echo "Reuse prebuilt OpenCASCADE 7.9.3."
fi

# By using TOPOLOGIC_OUTPUT_ID environment variable, it's possible to collect
# variables printed by setup.py to file "${TOPOLOGIC_OUTPUT_ID}.log".
export TOPOLOGIC_OUTPUT_ID=${RANDOM}${RANDOM}${RANDOM}
TOPOLOGIC_OUTPUT_FILE_PATH=${PWD}/TopologicPythonBindings/${TOPOLOGIC_OUTPUT_ID}.log
trap '{ rm -f -- "$TOPOLOGIC_OUTPUT_FILE_PATH"; }' EXIT
export TOPOLOGIC_PLAT_NAME=manylinux2014_x86_64

for PYVER in 3.14 3.13 3.12 3.11 3.10 3.9 3.8
do
  PYCPVER=cp${PYVER/./}
  PYDIR=/opt/python/${PYCPVER}-${PYCPVER}
  PYBIN=${PYDIR}/bin

  ls -l ${PYBIN} || true
  rm -fr TopologicPythonBindings/{build,dist,topologic_core.egg-info}/  build/
  rm -f -- "$TOPOLOGIC_OUTPUT_FILE_PATH"
  # Add Python tools required by 'repair_wheel_linux.py'.
  "${PYBIN}/pip" install --upgrade --force setuptools
  "${PYBIN}/pip" install wheel auditwheel patchelf delocate

  # Parse Python version in format for the project.
  PYTHONVER=`"${PYBIN}/python" -c 'import sys; print(f"{sys.version_info[0]}{sys.version_info[1]}")'`
  echo "Parsed PYTHONVER is ${PYTHONVER}"

  cd TopologicPythonBindings

  export TOPOLOGIC_EXTRA_CMAKE_ARGS="-DOpenCASCADE_DIR=${OCCT_INSTALL_DIR}/lib/cmake/opencascade \
    -DPython_LIBRARY=${PYDIR}/lib/libpython${PYVER}.so \
    -DPython_INCLUDE_DIR=${PYDIR}/include/python${PYVER}/ \
    -DPython_EXECUTABLE=${PYDIR}/bin/python"
  echo "CMAKE extra arguments: ${TOPOLOGIC_EXTRA_CMAKE_ARGS}"

  # Custom LD_LIBRARY_PATH is required for 'repair_wheel_linux.py' to look at
  # the directory with all dependencies.
  LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OCCT_INSTALL_DIR}/lib \
      "${PYBIN}/python" build_linux.py

  # Obtain wheel name, e.g. "topologic-5.0.0-cp312-cp312-linux_x86_64.whl"
  WHEEL_NAME=$(grep --color=never -Po "^WHEEL_NAME=\K.*" "$TOPOLOGIC_OUTPUT_FILE_PATH")

  # In fact, the manylinux platform name is like
  # 'manylinux2014_x86_64.manylinux_2_17_x86_64'. So fix it.
  WHEEL_NAME=${WHEEL_NAME/${TOPOLOGIC_PLAT_NAME}/"${TOPOLOGIC_PLAT_NAME}.manylinux_2_17_x86_64"}

  # Run the tests.
  "${PYBIN}/pip" install "wheelhouse/${WHEEL_NAME}"
  cd test
  "${PYBIN}/python" topologictest01.py
  "${PYBIN}/python" topologictest02.py

  echo "Python ${PYCPVER} bindings have been built successfully."

  cd "${TOP_DIR}"

done

echo "All Python bindings have been built successfully."
