#!/bin/bash

# Exit on any error
set -e

REPO="https://github.com/replit-user/jmakepp"

echo "🔍 Checking for cross-compilers..."

# Install mingw-w64 for Windows cross-compilation
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "📦 Installing mingw-w64 for Windows cross-compiling..."
    if command -v apt &> /dev/null; then
        sudo apt update && sudo apt install -y mingw-w64
    elif command -v yum &> /dev/null; then
        sudo yum install -y mingw64-gcc-c++
    elif command -v dnf &> /dev/null; then
        sudo dnf install -y mingw64-gcc-c++
    elif command -v pacman &> /dev/null; then
        sudo pacman -S --noconfirm mingw-w64-gcc
    else
        echo "❌ Unsupported package manager. Please install mingw-w64 manually."
        exit 1
    fi
else
    echo "✅ mingw-w64 already installed"
fi

# Clone repo
git clone "$REPO" ./clone
cd clone

# Verify binary exists
if [ ! -f ./bin/jmakepp ]; then
    echo "❌ Error: jmakepp binary not found in repo!"
    exit 1
fi
chmod +x ./bin/jmakepp
# Backup existing binary if present
if [ -f /usr/bin/jmakepp ]; then
    sudo cp /usr/bin/jmakepp /usr/bin/jmakepp.backup
    echo "📦 Backed up existing jmakepp to /usr/bin/jmakepp.backup"
fi
# Move binary
sudo mv ./bin/jmakepp /usr/bin/jmakepp
echo "✅ jmakepp installed to /usr/bin/jmakepp"
echo "Try it with: jmakepp version, should output 1.8.2"

cd ..
rm -rf ./clone
