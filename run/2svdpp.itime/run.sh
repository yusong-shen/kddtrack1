#!/bin/bash
echo "Running Experiment Procedure for KDDCup Track1 using SVDFeature"
echo "---------------------------------------"
echo "   MODEL: SVD++ item time bias "
echo "---------------------------------------"
echo "Build the binary buffer files used by SVDFeature, procedures in Makefile"
echo "This can take long time ..."
cd ../../
make buffer.train.svdpp.itime
make buffer.test.svdpp.itime
echo "Training using SVDFeature, evaluation after each round, output RMSE..."
cd run/2svdpp.itime
rm -f log.txt
# run for 50 rounds
../run_kddcup.sh 0 50



