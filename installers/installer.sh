#!/bin/bash

# Exit on any error
set -e

REPO="https://github.com/replit-user/jmakepp"

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
if [ ! -f ./bin/jmakepp ]; then
    echo "❌ Error: jmakepp binary not found in repo!"
    exit 1
fi
chmod +x ./bin/jmakepp
# Move binary
sudo mv ./bin/jmakepp /usr/bin/jmakepp
echo "✅ jmakepp installed to /usr/bin/jmakepp"
echo "Try it with: jmakepp version, should output 1.8.2"

cd ..
rm -rf ./clone
