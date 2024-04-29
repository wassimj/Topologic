#!/bin/bash
set -e

# Obtain project root/top directory and stay in it.
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"/..
TOP_DIR=$(pwd)
OCCT_INSTALL_DIR=${TOP_DIR}/occt-installed

# Parse optional arguments
vars=$(getopt -o on --long only-deps,no-deps -- "$@")
eval set -- "$vars"

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

# Update APT and install tools.
sudo apt-get update
sudo apt-get install -y cmake ninja-build

# Show tools.
ninja --version
cmake --version

if [[ "$NO_DEPS" -ne 1 ]]; then
  # Build OpenCASCADE 7.8.1 (takes long time).
  if [ ! -d "${OCCT_INSTALL_DIR}" ]; then
    sudo apt-get install -y \
          rapidjson-dev \
          uuid-dev \
          tcllib tklib tcl-dev tk-dev libfreetype6-dev libxt-dev \
          libxmu-dev libgl1-mesa-dev libfreeimage-dev libtbb-dev \
          libgl2ps-dev

    git clone --depth 1 --branch V7_8_1 \
        https://git.dev.opencascade.org/repos/occt.git \
        "${TOP_DIR}/occt-sources"

    cd "${TOP_DIR}/occt-sources"
    cmake -B build -S . \
          -DCMAKE_BUILD_TYPE="Release" \
          -DINSTALL_DIR=${OCCT_INSTALL_DIR} \
          -D BUILD_MODULE_Draw:BOOL=OFF \
          -D USE_TBB:BOOL=OFF \
          -D BUILD_RELEASE_DISABLE_EXCEPTIONS=OFF \
          -D USE_FREEIMAGE:BOOL=ON \
          -D USE_RAPIDJSON:BOOL=ON \
          -D BUILD_RELEASE_DISABLE_EXCEPTIONS:BOOL=OFF

    cd "${TOP_DIR}/occt-sources/build"
    make -j$(nproc --ignore=2)
    make install
    cd "${TOP_DIR}"
    # Create backup for openCASCADE headers and precompiled libs, just in case.
    rm -fr occt-installed.tar.gz
    tar cvfz occt-installed.tar.gz ./occt-installed
  else
    echo "The directory '${OCCT_INSTALL_DIR}' for installed" \
         "openCASCADE exists. Remove the directory if you want to build" \
         "openCASCADE again (build takes long time)."
  fi

  if [[ "$ONLY_DEPS" -eq 1 ]]; then
    echo "Only dependencies. No need to build the project."
    exit 0
  fi
fi

# Add Python tools required by 'repair_wheel_linux.py'.
pip install auditwheel patchelf delocate

# Parse Python version in format for the project.
PYTHONVER=`python -c 'import sys; print(f"{sys.version_info[0]}{sys.version_info[1]}")'`
echo "Parsed PYTHONVER is ${PYTHONVER}"

# Build the project.
export TOPOLOGIC_EXTRA_CMAKE_ARGS=-DOpenCASCADE_DIR=${OCCT_INSTALL_DIR}/lib/cmake/opencascade
cd TopologicPythonBindings
# Custom LD_LIBRARY_PATH is required for 'repair_wheel_linux.py' to look at
# the directory with all dependencies.
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${OCCT_INSTALL_DIR}/lib \
    python build_linux.py

# Run the tests.
pip install wheelhouse/topologic-5.0.0-cp${PYTHONVER}-cp${PYTHONVER}-linux_x86_64.whl
cd test
python topologictest01.py
python topologictest02.py

echo "Python bindings have been built successfully."
