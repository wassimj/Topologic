(For installation instructions, scroll to the bottom of the page)
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

### Installation Instructions for Windows 10
This projects builds TopologicCore from the C++ sources (available at https://github.com/wassimj/Topologic.git)

The instructions below are for Microsoft Windows 10. In these instructions we assume *Visual Studio Community 2017* *opencascade 7.4.0*. We also assume that your account has Adminstrator priviliges.

1. **Create a topologicbim working folder**: We will assume that your home folder is called *homefolder* and you will install everything in *homefolder*/topologicbim

2. **Install Visual Studio Community 2019 with python and git**

Download from https://visualstudio.microsoft.com/downloads/
Make sure you check the box for Desktop Development with C++ (Windows category)
Make sure you install the correct version of Python (Web Development category)
Make sure you install git: https://www.oreilly.com/library/view/mastering-visual-studio/9781787281905/61432953-a0b3-4b16-b1fb-2636f7271582.xhtml#:~:text=To%20install%20the%20Git%20plugin,GitHub%20extension%20for%20Visual%20Studio.

3. **Install cmake and cppyy**

```
python -m venv topologic
topologic\Scripts\activate
(topologic) > python -m pip install cmake cppyy --upgrade
```

4. **Install Opencascade 7.4.0**

Download from https://old.opencascade.com/content/previous-releases

Choose  *Windows installer VC++ 2017 64 bit: opencascade-7.4.0-vc14-64.exe (258 336 552 bytes)*

This will automatically install opencascade in:
```
C:/OpenCASCADE-7.4.0-vc14-64
```
Do **NOT** change the location and name of this folder.

5. **Install Topologic**

* Go to the Start Menu in the lower left corner
* Search for the Visual Studio 2017 Folder and expand it
* Choose *x64 Native Tools Command Prompt*
* In the window that appears type:
```
cd C:/Users/*homefolder*/topologicbim
git clone https://github.com/wassimj/Topologic.git
cd Topologic
WindowsBuild.bat
```
6. **Set the Environment Variable**

A window will open with a folder that has all the DLL files. Copy the path of this folder (e.g. C:\Users\*homefolder*\topologicbim\Topologic\output\x64\Release) and add it to the **PATH** environment variable:
```
. In Search, search for and then select: System (Control Panel)
. Click the Advanced system settings link.
. Click Environment Variables. ...
. In the Edit System Variable (or New System Variable) window, add the folder to the PATH environment variable.
