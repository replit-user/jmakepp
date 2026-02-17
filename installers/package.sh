#!/bin/bash
set -e

VERSION=$(grep '"version"' ../project.json | sed 's/.*"version": "\([^"]*\)".*/\1/')

echo "📦 Packaging jmakepp version $VERSION"

# --- Create .deb package ---
echo "🔨 Creating .deb package..."
mkdir -p deb-package/DEBIAN deb-package/usr/bin
cp ../bin/jmakepp_linux deb-package/usr/bin/jmakepp
chmod 755 deb-package/usr/bin/jmakepp

cat > deb-package/DEBIAN/control << EOF
Package: jmakepp
Version: $VERSION
Architecture: amd64
Maintainer: replit-user
Description: Simple C++ Build Tool
Depends: g++, mingw-w64
Recommends: mingw-w64
EOF

dpkg-deb --build deb-package "jmakepp_${VERSION}_amd64.deb"
rm -rf deb-package

# --- Create .msi package (using wixl) ---
echo "🔨 Creating .msi package with wixl..."

if ! command -v wixl >/dev/null 2>&1; then
  echo "❌ wixl (msitools) not found. Install with: sudo apt install msitools"
  exit 1
fi

mkdir -p wix
cat > wix/jmakepp.wxs << EOF
<?xml version="1.0" encoding="UTF-8"?>
<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">
  <Product
    Id="*"
    Name="jmakepp"
    Language="1033"
    Version="${VERSION}"
    Manufacturer="replit-user"
    UpgradeCode="$(uuidgen)">
    
    <Package InstallerVersion="500" Compressed="yes" InstallScope="perMachine"/>
    <MajorUpgrade DowngradeErrorMessage="A newer version of jmakepp is already installed."/>
    
    <!-- Allow user to choose installation folder -->
    <Property Id="INSTALLDIR" Secure="yes" />
    
    <Media Id="1" Cabinet="jmakepp.cab" EmbedCab="yes" />
    
    <Feature Id="MainFeature" Title="jmakepp" Level="1">
      <ComponentRef Id="MainExecutable"/>
    </Feature>

    <Directory Id="TARGETDIR" Name="SourceDir">
      <Directory Id="ProgramFilesFolder">
        <Directory Id="INSTALLDIR" Name="jmakepp"/>
      </Directory>
    </Directory>

    <DirectoryRef Id="INSTALLDIR">
      <Component Id="MainExecutable" Guid="$(uuidgen)">
        <File Id="JmakeppExe" Source="../bin/jmakepp.exe" KeyPath="yes"/>
      </Component>
    </DirectoryRef>
  </Product>
</Wix>
EOF

wixl -o "jmakepp_${VERSION}_amd64.msi" wix/jmakepp.wxs
rm -rf wix



echo "✅ Packages created:"
echo "  - jmakepp_${VERSION}_amd64.deb"
echo "  - jmakepp_${VERSION}_amd64.msi"

echo ""
echo "⚠️ Notes:"
echo "  - Windows MSI: users can choose installation folder."