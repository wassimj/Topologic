# Build recipes

These are the scripts to build Topologic Python bindings for Linux and Windows.
Compiled files can be found in `TopologicPythonBindings/wheelhouse/`.


## Linux x86_64

### Using Conda: build_linux_x86_64_conda.sh

Installs Miniconda if it is not found in `$HOME/miniconda`. Builds Topologic
Python bindings for Linux with Miniconda. Currently, Miniconda runs with Python
up to v3.12. Below commands should work:

    ./build_linux_x86_64_conda.sh 312

    ./build_linux_x86_64_conda.sh 311

    ./build_linux_x86_64_conda.sh 310

    ./build_linux_x86_64_conda.sh 39

    ./build_linux_x86_64_conda.sh 38

### Without using Conda: build_linux_x86_64_noconda.sh

Builds Topologic Python bindings for Linux without Miniconda as it is not
shipped with Python greater than 3.12 yet. Assumed, you have Python installed
on your Linux machine, and it is added into your environment paths, so `pip`
just works in command line.

It will also build openCASCADE 7.8 what may take around 1h:

    build_linux_x86_64_noconda.sh

To build only openCASCADE:

    build_linux_x86_64_noconda.sh --only-deps

Do not build openCASCADE:

    build_linux_x86_64_noconda.sh --no-deps


## Windows amd64

### Using Conda: build_win_amd64_conda.bat

Installs Miniconda if it is not found in `C:\Miniconda`. Builds Topologic Python
bindings for Windows with Miniconda. Currently, Miniconda runs with Python up to
v3.12. Below commands should work:

    build_win_amd64_conda.bat /createenv 312
    # and
    build_win_amd64_conda.bat /useenv 312

    build_win_amd64_conda.bat /createenv 311
    # and
    build_win_amd64_conda.bat /useenv 311

    build_win_amd64_conda.bat /createenv 310
    # and
    build_win_amd64_conda.bat /useenv 310

    build_win_amd64_conda.bat /createenv 39
    # and
    build_win_amd64_conda.bat /useenv 39

    build_win_amd64_conda.bat /createenv 38
    # and
    build_win_amd64_conda.bat /useenv 38


### Without using Conda: build_win_amd64_noconda.bat

Builds Topologic Python bindings for Linux without Miniconda as it is not
shipped with Python greater than 3.12 yet. Assumed, you have Python installed
on your Windows machine, and it is added into your environment paths, so `pip`
just works in command line.

It will use compiled openCASCADE 7.8 for Windows AMD64 from the official portal.

    build_win_amd64_noconda.bat
