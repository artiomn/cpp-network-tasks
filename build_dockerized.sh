#!/bin/sh

docker run -it --rm -v "$PWD:/usr/src/gb" -e "EXT_UID=$(id -u)" -e "EXT_GID=$(id -g)" -w /usr/src/gb artiomn/gb-build-image ./build.sh $*

