#!/bin/bash -xe
mkdir -p build-release
cd build-release
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
cpack
dpkg -c ./*.deb