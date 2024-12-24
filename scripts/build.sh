#!/bin/env sh

INTERNALLINKFILES=`./scripts/linkfiles.sh`

ARGUMENTS=$1
FILE=$2

if [ -z $FILE ] 
then
    FILE="./jadeled.c";
fi

GRAPHICS_MAGICK_ARGS=`GraphicsMagick-config --cppflags --ldflags --libs`
g++ $ARGUMENTS $FILE $INTERNALLINKFILES $GRAPHICS_MAGICK_ARGS -I../rpi-rgb-led-matrix/include -I/usr/include/libxml2/ -I/lib/aarch64-linux-gnu/dbus-1.0/include/ -I/usr/include/dbus-1.0/ -L../rpi-rgb-led-matrix/lib -lws2811 -ldbus-1 -lpigpio -lrgbmatrix -lrt -lm -lpthread -lxml2

return $?
