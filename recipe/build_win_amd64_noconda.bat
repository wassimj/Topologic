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

:: Below is required for 'repair_wheel_windows.py'.
pip install machomachomangler pefile delocate build || goto :ERROR
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

:: By using TOPOLOGIC_OUTPUT_ID environment variable, it's possible to collect
:: variables printed by setup.py to file "${TOPOLOGIC_OUTPUT_ID}.log".
set TOPOLOGIC_OUTPUT_ID=%RANDOM%%RANDOM%%RANDOM%%RANDOM%%RANDOM%%RANDOM%
set TOPOLOGIC_OUTPUT_FILE_PATH=%CD%\%TOPOLOGIC_OUTPUT_ID%.log

:: Build the project.
python build_windows.py || goto :ERROR

:: Obtain wheel name, e.g. "topologic-5.0.0-cp312-cp312-linux_x86_64.whl"
:: and install it.
for /f "delims== tokens=1,2" %%G in (%TOPOLOGIC_OUTPUT_FILE_PATH%) do set %%G=%%H
pip install "wheelhouse/%WHEEL_NAME%" || goto :ERROR

:: Run tests
cd test || goto :ERROR
python topologictest01.py || goto :ERROR
python topologictest02.py || goto :ERROR

echo Python bindings have been built successfully.
goto :SUCCESS

:SUCCESS
if not [%TOPOLOGIC_OUTPUT_FILE_PATH%] == [] del /f /q "%TOPOLOGIC_OUTPUT_FILE_PATH%"
goto :EOF

:ERROR
echo Error #%errorlevel%
if not [%TOPOLOGIC_OUTPUT_FILE_PATH%] == [] del /f /q "%TOPOLOGIC_OUTPUT_FILE_PATH%"
echo Quit with error
exit /b 1
