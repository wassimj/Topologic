# Building Python Bindings for Topologic from Source

## Requirements:
* Opencascade 0.7.4 installed in `C:\OpenCASCADE-7.4.0-vc14-64`
* python 3.9.X
* Microsoft Visual Studio 2019 with C++ Desktop Development Tools option installed

## Steps
1. Launch the x64 Native Tools Command Prompt for VS 2019
2. Change your directory to your home directory
3. Create a new folder in your home directory to store the project in. We will call it `topologicbim`
```
	mkdir topologicbim
```
4. Clone the Topologic github repository
```
	git clone https://github.com/wassimj/Topologic.git
```
5. Change the directory to `Topologic`
```
	cd Topologic
```
6. Run the `WindowsBuild.bat` file
```
	WindowsBuild.bat
```
7. Keep the resulting window open (`topologicbim\Topologic\output\x64\Release`)
8. Change the directory to Python-Bindings
```
	cd Python-Bindings
```
9. Create a new folder called `build`
```
	mkdir build
```
10. Change the directory to `build`
```
	cd build
```
11. Issue the following `cmake` command
```
	cmake -Ax64 -DCMAKE_BUILD_TYPE=Release ..
```
12. Issue the following `cmake` command
```
	cmake --build . --config Release
```
13. Open the `Python-Bindings\build\Release` folder
14. Copy the three files in it to the folder specified in Step 7

## Testing
1. Change the directory to the `test` folder
```
	cd ..\test
```
2. Edit the `topologictest01.py` file in the test folder. Change the file path on line 2 to reflect your actual path to the folder specified in Step 7
3. Back in the command prompt, issue the following command
```
	python topologictest01.py
```
4. You should see a `Congratulations! Test is successful.` print out at the end of the test.
	
