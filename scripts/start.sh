#!/bin/env sh

if [ -z $1 ]
then
    MAIN="./jadeled.c"
else
    MAIN="$1"
fi
clear;
echo "[ 🔨 ] Building...";
if ./scripts/build.sh "-o ./jadeled" $MAIN
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
