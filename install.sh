#!/bin/sh

sudo apt install git

git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build

cd ../
sudo apt install libpcap-dev

#gcc

sudo apt install gcc

gcc main.c -o vlan_checker -lpcap -I/usr/local/include -L/usr/local/lib -lwiringPi


sudo cp vlan_checker /usr/bin/


#service
sudo cp ./vlan_checker.service /etc/systemd/system/

sudo systemctl enable vlan_checker.service
