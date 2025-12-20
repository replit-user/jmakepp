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

# --- Create macOS .app bundle ---
echo "🔨 Creating macOS .app bundle..."
APP_DIR="jmakepp.app"
mkdir -p "$APP_DIR/Contents/MacOS" "$APP_DIR/Contents/Resources"
cp ../bin/jmakepp_macos "$APP_DIR/Contents/MacOS/jmakepp"
chmod +x "$APP_DIR/Contents/MacOS/jmakepp"

cat > "$APP_DIR/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>
    <string>jmakepp</string>
    <key>CFBundleDisplayName</key>
    <string>jmakepp</string>
    <key>CFBundleIdentifier</key>
    <string>com.replit.jmakepp</string>
    <key>CFBundleVersion</key>
    <string>${VERSION}</string>
    <key>CFBundleExecutable</key>
    <string>jmakepp</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
</dict>
</plist>
EOF

# --- Create macOS .pkg installer ---
echo "🔨 Creating macOS .pkg installer..."
PKG_ROOT="pkgroot"
PKG_IDENTIFIER="com.replit.jmakepp"
mkdir -p "$PKG_ROOT/usr/local/bin"
cp ../bin/jmakepp_macos "$PKG_ROOT/usr/local/bin/jmakepp"
chmod +x "$PKG_ROOT/usr/local/bin/jmakepp"

./pkgbuild/PKGBUILD --root "$PKG_ROOT" \
         --identifier "$PKG_IDENTIFIER" \
         --version "$VERSION" \
         --install-location "/usr/local/bin" \
         "jmakepp_${VERSION}_macos.pkg"

rm -rf "$PKG_ROOT"

echo "✅ Packages created:"
echo "  - jmakepp_${VERSION}_amd64.deb"
echo "  - jmakepp_${VERSION}_amd64.msi"
echo "  - jmakepp.app"
echo "  - jmakepp_${VERSION}_macos.pkg"

echo ""
echo "⚠️ Notes:"
echo "  - Windows MSI: users can choose installation folder."
echo "  - macOS PKG: users can choose installation location during install."
echo "  - macOS .app: drag-and-drop runnable bundle."
