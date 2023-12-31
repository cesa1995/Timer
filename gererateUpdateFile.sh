#!/bin/bash

if [ ! -e "./version.txt"  ]
then
    touch ./version.txt
fi

COMMIT=$(git log -n 1 --pretty=format:%s )

tag=$( tail -n 2 version.txt )
LASTCOMMIT=${tag#*=}
LASTCOMMIT=${LASTCOMMIT%%:*}

if [[ $LASTCOMMIT != $COMMIT ]] || [[ $LASTCOMMIT == "" ]] 
then
    pio run --environment esp32dev
    mkdir ./temp/
    LASTVERSION=${tag##*=}
    VERSION="$((LASTVERSION + 1))"
    LASTFILE="./update_$LASTVERSION.tgz"

    if [ -e $LASTFILE  ]
    then
        rm $LASTFILE
    fi

    echo "commit=$COMMIT:" >> version.txt
    echo "version=$VERSION" >> version.txt
    
    cp -r ./data/* ./temp/
    cp ./.pio/build/esp32dev/firmware.bin ./temp/
    cp version.txt ./temp/

    cd ./temp/
    
    tar -czf ../update_$VERSION.tgz * 
    rm -r ../temp/
else
    echo "Commit already versionade"
fi



