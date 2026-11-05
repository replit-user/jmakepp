set -e
version=$(grep '"version"' ./project.json | sed 's/.*"version": "\([^"]*\)".*/\1/')
jmakepp clean
jmakepp build $version
mv ./bin/*.exe ./bin/jmakepp.exe
rm -f ./installers/*.msi && rm -f ./installers/*.deb && rm -f ./installers/*.pkg.tar.zst
./installers/package.sh
gpg -b ./bin/jmakepp_linux && gpg --verify ./bin/jmakepp_linux.sig ./bin/jmakepp_linux && cd ..
