cd bin && rm -f ./* && cd ..
version="2.0.4"
jmakepp build "$version"
cd bin && mv ./*.exe ./jmakepp.exe && cd ..
cd installers && ./package.sh && cd ..
cd bin && gpg -b jmakepp_linux && gpg --verify jmakepp_linux.sig jmakepp_linux && cd ..