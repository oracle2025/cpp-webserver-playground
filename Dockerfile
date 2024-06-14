FROM debian:11.7

RUN set -ex \
    && sed -i -- 's/# deb-src/deb-src/g' /etc/apt/sources.list \
    && apt-get update \
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
    && apt-get clean \
    && rm -rf /tmp/* /var/tmp/*