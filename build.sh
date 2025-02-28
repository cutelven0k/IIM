#!/bin/bash

set -e

# Detect operating system
if [ -f /etc/os-release ]; then
    source /etc/os-release
    OS_NAME=$ID
    OS_VERSION=$VERSION_ID
else
    echo "Error: Unable to detect operating system. make sure that /etc/os-release exists."
    exit 1
fi

# Probably Ubuntu version
case "$OS_NAME" in
    ubuntu)
        case "$OS_VERSION" in
            "24.04")
                DEPS_FILE="https://raw.githubusercontent.com/userver-framework/userver/refs/heads/develop/scripts/docs/en/deps/ubuntu-24.04.md"
                ;;
            "22.04")
                DEPS_FILE="https://raw.githubusercontent.com/userver-framework/userver/refs/heads/develop/scripts/docs/en/deps/ubuntu-22.04.md"
                ;;
            "20.04")
                DEPS_FILE="https://raw.githubusercontent.com/userver-framework/userver/refs/heads/develop/scripts/docs/en/deps/ubuntu-20.04.md"
                ;;
            *)
                echo "Unsupported Ubuntu version: $OS_VERSION"
                exit 1
                ;;
        esac
        ;;
    *)
        echo "Unsupported OS: $OS_NAME,\nYou can add support for this OS (maybe), check: https://userver.tech/de/db9/md_en_2userver_2build_2dependencies.html"
        exit 1
        ;;
esac

# Install dependencies
echo "Using dependency file:: $DEPS_FILE"
sudo apt update
sudo apt install --allow-downgrades -y $(wget -q -O - ${DEPS_FILE})
echo "✅ Dependencies installed successfully!"

mkdir -p build && cd build
cmake ..
cmake --build .