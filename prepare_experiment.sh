#!/bin/bash
# modify the following line: set the right path to SVDFeature
SVDFEATURE=/home/ys43/svdfeature-1.2.2/

if [ -f $SVDFEATURE/svd_feature.cpp ]
then
    echo "SVDFeature Path="$SVDFEATURE
else
    echo "error: please modify 3rd line of prepare_experiment.sh to the the right path to SVDFeature"
    exit -1
fi
# start 
echo "Make necessary folders and compile codes, set svd-feature to the right folder"
ln -s $SVDFEATURE svd-feature
cd svd-feature; make; cd ..
cd utils; make ; cd ..
mkdir features
cd cpp 
make 
cd ..
echo "prepare_experiment finish runing"

