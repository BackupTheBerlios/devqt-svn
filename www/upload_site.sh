#! /bin/sh

rsync -vzr --delete --exclude '.svn' --exclude '*.sh' ./ elcuco@shell.berlios.de:/home/users/elcuco/devqt/
