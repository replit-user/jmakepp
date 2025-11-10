#!/bin/bash
set -e

VERSION=$(grep '"version"' ../project.json | sed 's/.*"version": "\([^"]*\)".*/\1/')

echo "📦 Packaging jmakepp version $VERSION"

# --- Create .deb package ---
echo "🔨 Creating .deb package..."
mkdir -p deb-package/DEBIAN deb-package/usr/bin
cp ../bin/jmakepp deb-package/usr/bin/jmakepp
chmod 755 deb-package/usr/bin/jmakepp

cat > deb-package/DEBIAN/control << EOF
Package: jmakepp
Version: $VERSION
Architecture: amd64
Maintainer: replit-user
Description: Simple C++ Build Tool
Depends: g++, mingw-w64
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
    
    <!-- Add Media and MediaTemplate elements to ensure the cabinet is created -->
    <Media Id="1" Cabinet="jmakepp.cab" EmbedCab="yes" />
    
    <!-- Feature definition -->
    <Feature Id="MainFeature" Title="jmakepp" Level="1">
      <ComponentRef Id="MainExecutable"/>
    </Feature>

    <!-- Directory structure -->
    <Directory Id="TARGETDIR" Name="SourceDir">
      <Directory Id="ProgramFilesFolder">
        <Directory Id="INSTALLFOLDER" Name="jmakepp"/>
      </Directory>
    </Directory>

    <!-- Install executable -->
    <DirectoryRef Id="INSTALLFOLDER">
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
echo "⚠️ Note: PATH is not automatically updated (wixl doesn't support that)."
echo "         You can manually add the install folder to PATH after installation."
