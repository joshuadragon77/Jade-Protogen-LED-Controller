#!/bin/env sh

if [ -z $1 ]
then
    MAIN="./jadeos.c"
else
    MAIN="$1"
fi
clear;
echo "[ 🔨 ] Building...";
if ./scripts/build.sh "-o ./jadeos" $MAIN
then
    echo "[ 🔨 ] Build Success!";
    sleep 1;
    echo "[ ✅ ] Running JadeOS..."
    sleep 1;
    ./scripts/run.sh
    echo "[ 👋 ] Protogen Out. o7"
else
    echo "[ ❌ ] Failed Build";
fi
