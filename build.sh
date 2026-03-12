cd bin && rm -f -r ./* && cd ..
version="2.0.5"
jmakepp build "$version"
cd bin && mv ./*.exe ./jmakepp.exe && cd ..
cd installers && ./package.sh && cd ..
cd bin && gpg -b jmakepp_linux && gpg --verify jmakepp_linux.sig jmakepp_linux && cd ..
cd bin
gpg -s jmakepp_linux
