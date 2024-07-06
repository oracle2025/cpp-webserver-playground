#!/bin/bash -xe
docker build -t cpp_webserver_kata .
docker run -t -v $PWD:/srv -w/srv cpp_webserver_kata /bin/bash -c "./scripts/build-release.sh"