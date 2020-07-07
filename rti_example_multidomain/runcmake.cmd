mkdir build
cd build
cmake -DCONNEXTDDS_DIR="C:\Program Files\rti_connext_dds-6.0.1" -DCONNEXTDDS_ARCH=x64Win64VS2017 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON ../
cmake --build .
pause