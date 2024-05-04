#!/bin/bash
set -e

# Determine Python version. Corresponding conda_env_topologic_py*.yml
# must exist.
PYTHONVER=312
if [ ! -z "$1" ]; then PYTHONVER=$1; fi
set --

# Obtain project root/top directory and stay in it.
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"/..
TOP_DIR=$(pwd)

# Install and activate Miniconda.
if [ ! -d "$HOME/miniconda" ]; then
  wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda.sh
  bash ~/miniconda.sh -b -p $HOME/miniconda
fi
source ~/miniconda/bin/activate

# Make sure Miniconda environment is created.
if conda info --envs | grep -q topologic_py${PYTHONVER}; then
  echo "Well done, conda environment already created."
else
  conda env create -f conda_env_topologic_py${PYTHONVER}.yml
fi

# Activate Miniconda environment.
conda activate topologic_py${PYTHONVER}
python --version

cd TopologicPythonBindings

# By using TOPOLOGIC_OUTPUT_ID environment variable, it's possible to collect
# variables printed by setup.py to file "${TOPOLOGIC_OUTPUT_ID}.log".
export TOPOLOGIC_OUTPUT_ID=${RANDOM}${RANDOM}${RANDOM}
TOPOLOGIC_OUTPUT_FILE_PATH=${PWD}/${TOPOLOGIC_OUTPUT_ID}.log
trap '{ rm -f -- "$TOPOLOGIC_OUTPUT_FILE_PATH"; }' EXIT

# Build the project.
python build_linux.py

# Obtain wheel name, e.g. "topologic-5.0.0-cp312-cp312-linux_x86_64.whl"
WHEEL_NAME=$(grep --color=never -Po "^WHEEL_NAME=\K.*" "$TOPOLOGIC_OUTPUT_FILE_PATH")

# Run the tests.
pip install "wheelhouse/${WHEEL_NAME}"
cd test
python topologictest01.py
python topologictest02.py

echo "Python bindings have been built successfully."
