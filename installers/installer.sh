#!/bin/bash

# Validate repo URL (replace with actual URL)
REPO="https://github.com/replit-user/sbuild" 

# Exit on errors
set -e

# Clone repo
git clone "$REPO" ./clone
cd clone

# Verify binary exists
if [ ! -f ./bin/sbuild ]; then
    echo "Error: sbuild binary not found in repo!"
    exit 1
fi

# Move binary (foreground operation)
sudo mv ./bin/sbuild /bin/sbuild

echo "sbuild installed to /bin/sbuild"
cd ..
rm -rf ./clone