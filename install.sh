#!/bin/bash

mkdir build
cd build
cmake ..
make
cd ..

cp Antivirus/redAntivirus.service /etc/systemd/system/redAntivirus.service
cp build/Antivirus /usr/local/bin/Antivirus
