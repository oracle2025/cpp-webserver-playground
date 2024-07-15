#!/bin/bash -xe
mkdir -p build-release
cd build-release
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_USER_LIST=On -DENABLE_SIGNUP=On ..
ninja
cpack
dpkg -c ./*.deb