FROM gcc:11

MAINTAINER artiomn

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update \
  && apt-get -y --no-install-recommends --show-progress install \
    capnproto \
    clang \
    cmake \
    gdb \
    gdbserver \
    gosu \
    libboost-all-dev \
    qtbase5-dev \
    libgtest-dev \
    libglib2.0-dev \
    libgtk2.0-dev \
    libgtk-3-dev \
    libpcap-dev \
    libevent-2\* libevent-dev libev-dev libuv1-dev \
    libc-ares-dev \
    libpoco-dev \
    libtorrent-dev \
    inetutils-ping \
    iproute2 \
    less \
    meson \
    netcat \
    net-tools \
    protobuf-compiler \
    python \
    rapidjson-dev \
    rsync \
    sudo \
    ssh \
    tar \
    telnet \
    tcpdump \
    vim \
  && git clone --recursive --depth 1 https://github.com/artiomn/pistache \
  && cd pistache \
  && cmake \
    -DPISTACHE_USE_SSL=true \
    -DPISTACHE_BUILD_EXAMPLES=true \
    -DPISTACHE_BUILD_TESTS=false \
    -DPISTACHE_BUILD_DOCS=false \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    . \
  && cmake --build . --parallel \
  && cmake --install . --prefix "/usr/local" \
  && cd / \
  && rm -rf pistache \
  && git clone --recursive --depth 1 https://github.com/corvusoft/restbed.git \
  && cd restbed \
  && cmake -B build -DBUILD_SSL=YES -DBUILD_TESTS=NO -DCMAKE_INSTALL_PREFIX=/usr/local . \
  && cmake --build build --parallel \
  && cmake --install build --prefix "/usr/local" \
  && cd / \
  && rm -rf restbed \
  && git clone --recursive --depth 1 https://github.com/Thalhammer/jwt-cpp \
  && cd jwt-cpp \
  && cmake -B build -DBUILD_SSL=YES -DBUILD_TESTS=NO -DCMAKE_INSTALL_PREFIX=/usr/local . \
  && cmake --build build --parallel \
  && cmake --install build --prefix "/usr/local" \
  && rm -rf jwt-cpp \
  && cd / \
  && git clone --recursive --depth 1 https://github.com/reo7sp/tgbot-cpp \
  && cd tgbot-cpp \
  && cmake . \
  && cmake --build . --parallel \
  && cmake --install . --prefix "/usr/local" \
  && cd / \
  && rm -rf tgbot-cpp \
  && ldconfig \
  && apt-get clean

COPY ./entry.sh /
RUN chmod +x /entry.sh
COPY ./bashrc /etc/skel/.bashrc

ENTRYPOINT ["/entry.sh"]
CMD ["/bin/bash"]
