#!/bin/sh

SCRIPT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
SOURCE_PATH="${SOURCE_PATH:-/usr/src/gb}"
USER_NAME="${USER_NAME:-developer}"
export DISPLAY=":0"

groupadd -g "${EXT_GID}" "${USER_NAME}" && \
    useradd -m -u "${EXT_UID}" -g "${EXT_GID}" -Gsudo,root "${USER_NAME}" && \
    chown "${USER_NAME}:${USER_NAME}" "${SOURCE_PATH}" && \
    echo "${USER_NAME} ALL=(ALL) NOPASSWD: ALL" >> "/etc/sudoers.d/${USER_NAME}" && \
    exec gosu "${USER_NAME}" "$@"
