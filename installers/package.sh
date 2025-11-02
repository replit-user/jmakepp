#!/bin/bash

# Exit on any error
set -e

# Get version from project.json
VERSION=$(grep '"version"' ../project.json | sed 's/.*"version": "\([^"]*\)".*/\1/')

echo "📦 Packaging jmakepp version $VERSION"

# Create .deb package
echo "🔨 Creating .deb package..."
mkdir -p deb-package/DEBIAN deb-package/usr/bin
cp ../bin/jmakepp-1.8.3-linux deb-package/usr/bin/jmakepp
chmod 755 deb-package/usr/bin/jmakepp

# Create control file
cat > deb-package/DEBIAN/control << EOF
Package: jmakepp
Version: $VERSION
Architecture: amd64
Maintainer: replit-user
Description: Simple C++ Build Tool
Depends: g++
EOF

# Build .deb
dpkg-deb --build deb-package "jmakepp_${VERSION}_amd64.deb"
rm -rf deb-package

# Create .msi package (simple copy for now)
echo "🔨 Creating .msi package..."
cp ../bin/jmakepp.exe "jmakepp_${VERSION}_amd64.msi"

echo "✅ Packages created:"
echo "  - jmakepp_${VERSION}_amd64.deb"
echo "  - jmakepp_${VERSION}_amd64.msi"