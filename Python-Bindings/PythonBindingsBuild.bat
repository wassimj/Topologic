@mkdir build
@cd build
@cmake -Ax64 -DCMAKE_BUILD_TYPE=Release ..
@cmake --build . --config Release
@start Release
@cd ..