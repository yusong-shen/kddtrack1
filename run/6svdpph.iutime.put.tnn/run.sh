#!/bin/bash
echo "Running Experiment Procedure for KDDCup Track1 using SVDFeature"
echo "---------------------------------------"
echo "   MODEL: SVD++ item time bias, user day bias, user temporal factor, taxonomy parent history, taxonomy aware factorization "
echo "---------------------------------------"
echo "Build the binary buffer files used by SVDFeature, procedures in Makefile"
echo "This can take long time ..."
cd ../../
make buffer.train.svdpph.iutime.put.tnn
make buffer.test.svdpph.iutime.put.tnn
echo "Training using SVDFeature, evaluation after each round, output RMSE..."
cd run/6svdpph.iutime.put.tnn
rm -f log.txt
# run for 50 rounds
../run_kddcup.sh 0 50
