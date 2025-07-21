#!/bin/bash

# Exit on any error
set -e

REPO="https://github.com/replit-user/sbuild"

echo "🔍 Checking for cross-compilers..."

# Install mingw-w64 for Windows cross-compilation
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "📦 Installing mingw-w64 for Windows cross-compiling..."
    sudo apt update && sudo apt install -y mingw-w64
else
    echo "✅ mingw-w64 already installed"
fi

# Clone repo
git clone "$REPO" ./clone
cd clone

# Verify binary exists
if [ ! -f ./bin/sbuild ]; then
    echo "❌ Error: sbuild binary not found in repo!"
    exit 1
fi

# Move binary
sudo mv ./bin/sbuild /usr/bin/sbuild
echo "✅ sbuild installed to /usr/bin/sbuild"
echo "Try it with: sbuild version, should output 1.7.1"

cd ..
rm -rf ./clone
