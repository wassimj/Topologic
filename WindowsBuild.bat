@mkdir build
@cd build
@cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release  ..
@nmake
@start ..\output\x64\Release
@cd ..