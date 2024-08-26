#!/bin/env sh

if [ -z $1 ]
then
    MAIN="./main.c"
else
    MAIN="$1"
fi

echo "[ 🔨 ] Building...";
if [ `./scripts/build.sh "-o ./jadeos" $MAIN` ]
then
    echo "[ 🔨 ] Build Success!";
    echo "[ ✅ ] Running JadeOS..."
    ./scripts/run.sh
    echo "[ 👋 ] Protogen Out. o7"
else
    echo "[ ❌ ] Failed Build";
fi
