#!/bin/bash

src=$1
file="$HOME/$src"
filerc="$HOME/.$(echo $SHELL | cut -f 3 -d "/")rc"
echo "alias $src='$file'" >> $filerc

exit 0
