FROM debian:12.6
# FROM debian:11.7
# docker run -t -v $PWD:/srv -w/srv -it cpp_webserver_kata /bin/bash
# docker build -t cpp_webserver_kata .

# RUN set -ex \
#     && sed -i -- 's/# deb-src/deb-src/g' /etc/apt/sources.list \
#     && apt-get update \
#     && apt-get install -y --no-install-recommends \
#                build-essential \
#                cmake \
#                libpoco-dev \
#                libelf-dev \
#                libdwarf-dev \
#                libspdlog-dev \
#                ninja-build \
#                git \
#                ca-certificates \
#     && apt-get clean \
#     && rm -rf /tmp/* /var/tmp/*

RUN set -ex \
    && apt-get update \
    && apt-get upgrade \
    && apt-get install -y --no-install-recommends \
               build-essential \
               cmake \
               libpoco-dev \
               libelf-dev \
               libdwarf-dev \
               libspdlog-dev \
               ninja-build \
               git \
               ca-certificates \
               libbackward-cpp-dev \
    && apt-get clean \
    && rm -rf /tmp/* /var/tmp/*