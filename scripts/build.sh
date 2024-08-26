#!/bin/env sh

INTERNALLINKFILES=`./scripts/linkfiles.sh`

ARGUMENTS=$1
FILE=$2

if [ -z $FILE ] 
then
    FILE="./main.c";
fi

g++ $ARGUMENTS $FILE $INTERNALLINKFILES `GraphicsMagick-config --cppflags --ldflags --libs` -I../rpi-rgb-led-matrix/include -I/usr/include/libxml2/ -L../rpi-rgb-led-matrix/lib -lrgbmatrix -lrt -lm -lpthread -lxml2

echo "Sucess";