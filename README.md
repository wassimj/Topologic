**Table of Contents**

1. Description (https://github.com/wassimj/Topologic/blob/main/README.md#Topologic)
2. Installation Instructions:
     1. Windows 10: (https://github.com/wassimj/Topologic/blob/main/README.md#installation-instructions-for-windows-10)
     2. Linux: (https://github.com/wassimj/Topologic/blob/main/README.md#installation-instructions-for-Linux)
     3. MacOS: (https://github.com/wassimj/Topologic/blob/main/README.md#installation-instructions-for-MacOS)

# Topologic
Topologic is a software modelling library enabling hierarchical and topological representations of architectural spaces, buildings and artefacts through non-manifold topology.

The project website is located here: https://topologic.app/


## Introduction
Topologic is a software modelling library enabling hierarchical and topological representations of architectural spaces, buildings and artefacts through non-manifold topology (NMT). Topologic is designed as a core library and additional plugins to visual data flow programming (VDFP) applications and parametric modelling platforms commonly used in architectural design practice. These applications provide workspaces with visual programming nodes and connections for architects to interact with Topologic and perform architectural design and analysis tasks.

Topologic is well-suited to create a lightweight representation of a building as an external envelope and the subdivision of the enclosed space into separate spaces and zones using zero-thickness internal surfaces. Because Topologic maintains topological consistency, a user can query these cellular spaces and surfaces regarding their topological data and thus conduct various analyses. For example, this lightweight and consistent representation was found to be well-matched with the input data requirements for energy analysis simulation software. Because Topologic allows entities with mixed dimensionalities and those that are optionally independent (e.g. a line, a surface, a volume) to co-exist, structural models can be represented in a coherent manner where lines can represent columns and beams, surfaces can represent walls and slabs, and volumes can represent solids. In addition, non-building entities, such as structural loads can be efficiently attached to the structure. This creates a lightweight model that is well-matched with the input data requirements for structural analysis simulation software.

## Examples of use
Toplogic can be used to support energy modelling - say dimensioning the windows on different sides of a hospital to avoid summer overheating
Topologic can be be used to plot paths such as fire egress routes, the least disruptive route for a new service pipe or the most congested location in a city layout

## Rigorous Class Hierarchy
TopologicCore contains the following main classes:

* Topology: A Topology is an abstract superclass that stores constructors, properties and methods used by other subclasses that extend it.
* Vertex: A Vertex is a zero-dimensional entity equivalent to a geometry point.
* Edge: An Edge is a one-dimensional entity defined by two vertices. It is important to note that while a topologic edge is made of two vertices, its geometry can be a curve with multiple control vertices.
* Wire: A Wire is a contiguous collection of Edges where adjacent Edges are connected by shared Vertices. It may be open or closed and may be manifold or non-manifold.
* Face: A Face is a two-dimensional region defined by a collection of closed Wires. The geometry of a face can be flat or undulating.
* Shell: A Shell is a contiguous collection of Faces, where adjacent Faces are connected by shared Edges. It may be open or closed and may be manifold or non-manifold.
* Cell: A Cell is a three-dimensional region defined by a collection of closed Shells. It may be manifold or non- manifold.
* CellComplex: A CellComplex is a contiguous collection of Cells where adjacent Cells are connected by shared Faces. It is non- manifold.
* Cluster: A Cluster is a collection of any topologic entities. It may be contiguous or not and may be manifold or non- manifold. Clusters can be nested within other Clusters.

# Installation Instructions for Windows 10
This projects builds TopologicCore from the C++ sources (available at https://github.com/wassimj/Topologic.git). If you just want to use Topologic (e.g. with Blender), you do not need to follow these instructions. Instead, just download the ZIP binaries from https://github.com/wassimj/TopologicSverchok/Releases

The instructions below are for Microsoft Windows 10. In these instructions we assume *Visual Studio Community 2019* *opencascade 7.4.0*. We also assume that your account has Administrator privileges.

1. **Install Opencascade 7.4.0**

Download from https://old.opencascade.com/content/previous-releases

Choose  *Windows installer VC++ 2017 64 bit: opencascade-7.4.0-vc14-64.exe (258 336 552 bytes)*

This will automatically install opencascade in:
```
C:\OpenCASCADE-7.4.0-vc14-64
```
Do **NOT** change the location and name of this folder.

2. **Create a topologicbim working folder**: We will assume that your home folder is called *homefolder* and you will install everything in *homefolder*/topologicbim

3. **Install Visual Studio Community 2019 with python 3.9 and git**

Download from https://visualstudio.microsoft.com/downloads/
Make sure you check the box for Desktop Development with C++ (Windows category)
Make sure you install the correct version of Python 3.9 (Web Development category)
Make sure you install git: https://www.oreilly.com/library/view/mastering-visual-studio/9781787281905/61432953-a0b3-4b16-b1fb-2636f7271582.xhtml#:~:text=To%20install%20the%20Git%20plugin,GitHub%20extension%20for%20Visual%20Studio.

4. **Install Topologic**

* Go to the Windows Start Menu in the lower left corner
* Search for the Visual Studio 2019 Folder and expand it
* Choose *x64 Native Tools Command Prompt*
* In the window that appears type:
```
cd C:\Users\*homefolder*\topologicbim
git clone https://github.com/wassimj/Topologic.git
cd Topologic
WindowsBuild.bat
```
5. **Set the Environment Variable**

A window will open with a folder that has all the DLL files. Copy the path of this folder (e.g. C:\Users\*homefolder*\topologicbim\Topologic\output\x64\Release) and add it to the **PATH** environment variable:

* In Search, search for and then select: System (Control Panel)
* Click the Advanced system settings link.
* Click Environment Variables. ...
* In the Edit System Variable (or New System Variable) window, add the folder to the PATH environment variable.

6. **Install Python bindings for Topolgic (Optional)**

* Prerequisite: WindowsBuild.bat have to be run without any errors
* Install Anaconda3 personal edition from https://www.anaconda.com/products/individual
* Launch Anaconda Navigator and click on the CMD.exe prompt.
* Within the window that appears Create a python 3.9 virtual environment. We will call it Blender392 because you can use that with Blender later. The name can be anything you want. Just always remember what you named your environment.

```
conda create --name Blender392 python=3.9.1
conda activate Blender392
```
* Install pybind11 within conda

```
conda install pybind11 -c conda-forge
```
* Install cmake

```
pip install cmake
```
* Switch to Python-Bindings folder and create a build folder inside
* Issue the normal CMake build command from the build folder

```
cd C:\Users\*homefolder*\topologicbim\Topologic\Python-Bindings
mkdir build
cd build
cmake -Ax64 -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```
7. **To make a distributable library:**

Place all the opencascade *.dll libraries, TopologicCore.dll and the python bindings (.pyd file) as above in one folder. ZIP that folder for distribution.

Note that you only need a handful of the opencascade *.dll files, not all of them. Our best guesstimate is that you need only the following opencascade files (removed the extension from the file name):

* TKBO
* TKBool
* TKBRep
* TKCAF
* TKCDF
* TKernel
* TKFillet
* TKG2d
* TKG3d
* TKGeomAlgo
* TKGeomBase
* TKIGES
* TKLCAF
* TKMath
* TKMesh
* TKOffset
* TKPrim
* TKShHealing
* TKTopAlgo
* TKXSBase

# Installation Instructions for Linux
This projects builds TopologicCore from the C++ sources (available at https://github.com/wassimj/Topologic.git)

The instructions below are for Ubuntu (Tested) and Fedors (Untested) Linux. We assume that your account has Administrator privileges.

1. **Install Opencascade 7.4.0 and dependencies**

*For Ubuntu (Tested)*
```
sudo apt-get install bzip2 unzip cmake make g++ git libgl-dev libglu-dev libpng-dev libxmu-dev libxi-dev libtbb-dev tcl-dev tk-dev zlib1g-dev libharfbuzz-dev libfreetype-dev libfreeimage-dev libocct-*-dev
```
if `libocct-*-dev` cannot be found while installing the dependencies, replace it with `libocct-foundation-dev libocct-data-exchange-dev`

*For Fedora (Untested. Suggested at OSArch.org)*
 ```
 sudo dnf install cmake gcc-c++ opencascade-devel libuuid-devel
 ```

2. **Create a topologicbim working folder**: We will assume that you will install everything in ~/topologicbim

3. **Install Topologic**

```
cd ~/topologicbim
git clone https://github.com/wassimj/Topologic.git
cd Topologic
mkdir build
cd build
cmake ..
make
sudo make install
```
At the end of this process, libTopologicCore.so should exist in /usr/local/lib

*For Fedora (Untested. Suggested at OSArch.org)*
```
cd ~/topologicbim
git clone https://github.com/wassimj/Topologic.git
cd Topologic
mkdir build
cd build
cmake ..
make
sudo make install
```
This will likely install the library and headers to /usr/local/lib so unless you have already edited your library path you need to do something like this:
```
sudo sh -c "echo /usr/local/lib >> /etc/ld.so.conf"
sudo ldconfig
```
4. **Install Python bindings for Topolgic (Optional)**

* Note: If you are planning to use Topologic with Blender and your system's python is different than the one Blender uses, then create a compatible python virtual environment. We will assume that Blender is using python 3.9.7 so the instructions below as you to create a conda virtual environment with python 3.9.7. We will call it py397. The name can be anything you want. Just always remember what you named your environment and match the python version to Blender's.

* Switch to Python-Bindings folder and create a build folder inside
* Issue the normal CMake build command from the build folder

```
cd ~/topologicbim/Topologic/Python-Bindings
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

If the above fails, try installing a conda environment and within it, install pybind11 first. Make sure you match the python version to what you need. Below we just assume python 3.9.7, but this could be different in your case.

```
conda create --name py397 python=3.9.7
conda activate py397
```
* Install pybind11 within conda

```
conda install pybind11 -c conda-forge
```

then try again:

```
cd ~/topologicbim/Topologic/Python-Bindings
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```
You can repeat the above process to build bindings for other versions of python (e.g. for 3.6, 3.7, and 3.8). You can bundle all the resulting .so files in the same distribution so they work with different python versions.

5. **To make a distributable library that can be installed in different locations on a variety of Linux systems:**

Compile and install all the opencascade and uuid *.so libraries, and build and install TopologicCore with the python bindings as above. 
Collect all the .so files, put them in one folder, cd to that folder and set the rpath, which is a magic incantation like this:

```
patchelf --set-rpath '$ORIGIN/.' *.so*
```

Strip debug symbols to make them smaller:

```
strip *.so*
```

Note that you only need a handful of the opencascade *.so files, not all of them. Our best guesstimate is that you need only the following files (removed the extension from the file name):

* TKBO
* TKBool
* TKBRep
* TKCAF
* TKCDF
* TKernel
* TKFillet
* TKG2d
* TKG3d
* TKGeomAlgo
* TKGeomBase
* TKIGES
* TKLCAF
* TKMath
* TKMesh
* TKOffset
* TKPrim
* TKShHealing
* TKTopAlgo
* TKXSBase

# Installation Instructions for MacOS

**(Raw instructions, badly formatted, needs work)**

This projects builds TopologicCore from the C++ sources (available at https://github.com/wassimj/Topologic.git)
These untested instructions are from Filipe Brandão <Filipe_Jorge_Brandao@iscte-iul.pt>

1. Build OpenCascade from binaries
     1. Download Open CASCADE Technology source package, tgz archive (opencascade-7.4.0.tgz) from: https://old.opencascade.com/content/previous-releases
     2. Get the needed third party libraries (the minimum requirements are Tcl8.5, Tk8.5 and FreeType2.4.10) You can get a precompiled version of all of them.
          1. ActiveTCL 8.5 includes both Tcl and Tk (https://www.activestate.com/products/tcl/)
          2. FreeType: https://dev.opencascade.org/resources/download/3rd-party-components
     3. Build: I used Cmake to generate the make files you can find detailed instructions here
          1.	Extract .tar file and create folder structure for Open Cascade
          ```
          tar -xf opencascade-7.4.0.tar
          tar -xf freetype-2.10.4-macos-x86_64.txz
          mkdir build
          ```
          2. Run cmake
          ```
          cd build
          cmake ../opencascade-7.4.0 -D3RDPARTY_FREETYPE_DIR=<freetype path>
          ```

          Optional parameters to be use in cmake:
          - ``-DCMAKE_OSX_DEPLOYMENT_TARGET=<target_macosx_version>`` to set your desired macosx version
          - ``-D3RDPARTY_TCL_DIR=<tcl_dir>`` (if you didn't install it with its installer)
          - ``-D3RDPARTY_TK_DIR=<tk_dir>`` (if you didn't install it with its installer)
          - ``-DCMAKE_INSTALL_PREFIX=<install_path>`` to change installation path
          3. Build
          
          ```
          make
          ```
          or for parallel build
          ```
          make -j<cpu_count>
          ```
          4. Install
          ```
          sudo make install
          ```

2.	Build TopologicCore from binaries
     1.	Clone repo and create folder structure for TopologicBim
     ```
     mkdir TopologicBim
     cd TopologicBim
     git clone https://github.com/wassimj/Topologic
     mkdir build
     cd build
     ```
     2. Run cmake
     ```
     cmake ../TopologicBim
     ```
     - You can set the location of OpenCascade binaries OCC_INCLUDE_DIR and gp_Pnt_hxx if you didn't install them into system with: ``-DOCC_INCLUDE_DIR=<occ_include_path>``and ``-Dgp_Pnt_hxx=<gp_Pnt_hxx_path>``
     - ``-DCMAKE_OSX_DEPLOYMENT_TARGET=<target_macosx_version>`` to set your desired macosx version
     - ``-DCMAKE_INSTALL_PREFIX=<install_path>`` to change installation path
     3. Build
     ```
     make
     ```
     or for parallel build
     ```
     make -j<cpu_count>
     ```
     4. Install
     ```
     sudo make install
     ```

3.	Build Python bindings
     1.	Create an environment with Anaconda, set python to the desired version and open a terminal
     
     ```
     conda create --name py397 python=3.9.7
     conda activate py397
     ```
     2. Generate folder structure
     ```
     cd TopologicBim/Python-Bindings
     mkdir build
     cd build
     
     cmake .. -DCMAKE_CXX_FLAGS=/usr/local/lib/
     ```
     3. Build
     ```
     make
     ```
     or for parallel build
     ```
     make -j<cpu_count>
     ```

4.	Make the files redistributable 

     Bring together all output files:
     - copy all files under ``<opencascade_build_dir>/mac64/clang/lib/`` into dist folder
     - copy all files under ``<TopologicCore_build_dir>/TopologicCore/libTopologicCorexxx.xxx.dylib`` into dist folder
     - copy ``topologic.cpython-39-darwin.so`` file into dist folder


In this last step you need to ensure that your executable topologic.cpython-39-darwin.so and all the other libraries are correctly configured

     1.	go to the final install dir of topologic where all the OpenCascade and Topologic dylib files and topologic.cpython-39-darwin.so are 
     2.	install_name_tool -add_rpath @loader_path topologic.cpython-39-darwin.so (we set the LC_RPATH of the executable)
     3.	check the which libraries are loaded by topologic.cpython-39-darwin.so using: otool -L topologic.cpython-39-darwin.so
     4.	install_name_tool -id @rpath topologic.cpython-39-darwin.so (change the id of the executable)
     5.	change all of the libraries OpenCascade and Topologic dylib paths to start with @rpath/... using: install_name_tool -change [old name] [new name] topologic.cpython-39-darwin.so
     6.	run renameLib.sh to change all the other libraries ids and paths


