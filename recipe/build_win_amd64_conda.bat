@echo off
setlocal ENABLEDELAYEDEXPANSION
cd /D "%~dp0.."

:: Determine Python version. Corresponding conda_env_topologic_py*.yml
:: must exist.
set PYTHONVER=%~2
if "%PYTHONVER%"=="" PYTHONVER=312

:: Decide which wget is to be used.
set WGET_BIN=C:\msys64\usr\bin\wget.exe
if not exist %WGET_BIN% (
  choco install wget --no-progress || goto :ERROR
  set WGET_BIN=wget.exe
)

:: Make sure Miniconda is installed.
if exist C:\Miniconda\NUL (
  echo Well done, Miniconda is already installed.
) else (
  echo Installing Miniconda with the help of wget.
  "%WGET_BIN%" https://repo.anaconda.com/miniconda/Miniconda3-latest-Windows-x86_64.exe || goto :ERROR
  start /wait "" Miniconda3-latest-Windows-x86_64.exe /InstallationType=JustMe ^
                 /AddToPath=0 /RegisterPython=0 ^
                 /S /D=C:\Miniconda || goto :ERROR
)
set PATH=%PATH%;C:\Miniconda\condabin
set CONDA_PREFIX=C:\Miniconda

:: First, call this script with '/createenv' argument to prepare conda env.
:: Then, call it with '/useenv' argument to build the project in conda env.
if "%~1"=="/createenv" (goto :CREATEENV)
if "%~1"=="/useenv" (goto :USEENV)
echo Error: Argument required /createenv or /useenv
exit \b 1

:CREATEENV
:: Note that 'conda env create' kills shell, no matter what. You would need to
:: continue by using /useenv argument.
conda env create -f conda_env_topologic_py%PYTHONVER%.yml || goto :ERROR
goto :EOF

:USEENV
:: Build the project in conda environment created by using '/createenv'.
echo Ready to build the project.
call C:\Miniconda\Scripts\activate.bat C:\Miniconda\envs\topologic_py%PYTHONVER%
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
