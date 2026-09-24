jmakepp clean
jmakepp build "2.1.1-bugfix"
mv ./bin/*.exe ./bin/jmakepp.exe
./installers/package.sh
gpg -b ./bin/jmakepp_linux && gpg --verify ./bin/jmakepp_linux.sig ./bin/jmakepp_linux && cd ..
