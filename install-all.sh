#!/bin/sh
sudo pacman -U ./dmenu-personalized/dmenu-personalized-4.5.13.5ed5e90-1-x86_64.pkg.tar.xz ./dwm-personalized/dwm-personalized-6.0.43.g14343e6-1-x86_64.pkg.tar.xz ./slock-class/slock-class-1.2-1-x86_64.pkg.tar.xz ./st-personalized/st-personalized-0.5-2-x86_64.pkg.tar.xz ./xombrero/xombrero3-1.6.4-1-x86_64.pkg.tar.xz
NC='\033[0m' # No Color
color='\033[1;35m'
printf "${color}Install dvtm with mod F13? (yes/no) : ${NC}"
read proceed
if [ "$proceed" = "yes" ]; then
	sudo pacman -U ./dvtm/dvtm-0.14-1-x86_64.pkg.tar.xz 
fi
