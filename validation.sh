#!/bin/bash

FILESTOCHECK="AliAOD.Muons.root"


validateout=`dirname $0`

if [ -z "$validateout" ]; then
    validateout="."
fi

cd "$validateout"
validateworkdir=`pwd`

(
echo "* *****************************************************"
echo "* AliRoot Validation Script V2.1                      *"
echo "* Time:    `date`"
echo "* Dir:     $validateout"
echo "* Workdir: $validateworkdir"
echo "* PATH: $PATH"
echo "* LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "* ----------------------------------------------------*"



echo "* ----------------------------------------------------*"
) >> stdout


error=0

for file in $FILESTOCHECK; do
    if [ ! -f "$file" ]; then
        error=1
        echo "* Error: Required file $file not found in the output" >> stdout
    fi
done

(
if [ $error -eq 0 ]; then
    echo "* ################   Job validated ####################"
else
    echo "*! ################   Job NOT validated, error code $error ################"

    if [ ! -z "$REMOVEROOTONERROR" ]; then
        echo "* ########## Removing all ROOT files from the local directory, leaving only the logs ###"
        rm -rf *.root
    fi
fi
) >> stdout

exit $error
