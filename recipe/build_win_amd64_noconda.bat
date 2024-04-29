@echo off
setlocal ENABLEDELAYEDEXPANSION
cd /D "%~dp0.."
set TOP_DIR=%cd%

:: Parse Python version in format for the project.
python --version || goto :ERROR
set PYTHONVER=
FOR /F "tokens=* USEBACKQ" %%g IN (
  `python -c "import sys; print(f"""{sys.version_info[0]}{sys.version_info[1]}""")"`
) do (
  set PYTHONVER=%%g
)
IF "%PYTHONVER%" == "" (
  echo Unable to parse Python version.
  goto :ERROR
)
echo Parsed PYTHONVER is %PYTHONVER%

:: Decide which wget is to be used.
set WGET_BIN=C:\msys64\usr\bin\wget.exe
if not exist %WGET_BIN% (
  choco install wget --no-progress || goto :ERROR
  set WGET_BIN=wget.exe
)

:CREATEENV
:: Download and extract official occt-vc143-64.
"%WGET_BIN%" https://github.com/Open-Cascade-SAS/OCCT/releases/download/V7_8_0/occt-vc143-64.zip || goto :ERROR
unzip occt-vc143-64.zip || goto :ERROR
:: Download and extract official third parties of occt-vc143-64.
"%WGET_BIN%" https://github.com/Open-Cascade-SAS/OCCT/releases/download/V7_8_0/3rdparty-vc14-64.zip || goto :ERROR
unzip 3rdparty-vc14-64.zip || goto :ERROR
:: Move dependencies in right place for Cmake.
mkdir D:\work\3rd-party_packages || goto :ERROR
move 3rdparty-vc14-64 D:\work\3rd-party_packages\win64 || goto :ERROR

:: Required for 'repair_wheel_windows.py'
pip install machomachomangler pefile delocate || goto :ERROR
:: One '--dll-dir' argument is supported in 'repair_wheel_windows.py', so
:: place all required DDLs in one dir.
echo Collecting all DLLs at one place.
copy D:\work\3rd-party_packages\win64\jemalloc-vc14-64\bin\jemalloc.dll ^
     %TOP_DIR%\occt-vc143-64\win64\vc14\bin || goto :ERROR
copy D:\work\3rd-party_packages\win64\tbb2021.5-vc14-x64\bin\*.dll ^
     %TOP_DIR%\occt-vc143-64\win64\vc14\bin || goto :ERROR

:USEENV
:: Build the project with dependencies.
echo Ready to build the project.
set TOPOLOGIC_EXTRA_CMAKE_ARGS=-DOpenCASCADE_DIR=%TOP_DIR%/occt-vc143-64/cmake -DCMAKE_CXX_STANDARD_LIBRARIES="ole32.lib /LIBPATH:D:/work/3rd-party_packages/win64/jemalloc-vc14-64/lib"
set TOPOLOGIC_EXTRA_DLL_DIR=%TOP_DIR%/occt-vc143-64/win64/vc14/bin
python --version || goto :ERROR
cd TopologicPythonBindings || goto :ERROR
python build_windows.py || goto :ERROR
pip install wheelhouse/topologic-5.0.0-cp%PYTHONVER%-cp%PYTHONVER%-win_amd64.whl || goto :ERROR
cd test || goto :ERROR
python topologictest01.py || goto :ERROR
python topologictest02.py || goto :ERROR

echo Python bindings have been built successfully.
goto :EOF

:ERROR
echo Error #%errorlevel%
exit \b 1
