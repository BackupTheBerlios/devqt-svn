#! /bin/sh

# 123
rsync -vzr --delete --exclude '.svn' --exclude '*.sh' --exclude "*~" ./ elcuco@shell.berlios.de:/home/users/elcuco/devqt/


