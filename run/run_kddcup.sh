#!/bin/bash

# run the SVDFeature training 

if [ $# -ne 2 ]
then
  echo "Usage: <start> <numround>"
  exit -1
fi

start=$1 
nround=$2

for(( i=$start; i < $nround; i ++ ))
do    
# run training, use different seed to ensure different kinds of pos/neg pair in each round
    ../../svd-feature/svd_feature config.conf max_round=1 num_round=$nround start_counter=$i continue=1 silent=1
    ../../svd-feature/svd_feature_infer config.conf focus=$((i+1)) silent=1 > stats.txt 
# record results in log
    cat stats.txt
    mv log.txt log 2>/dev/null
    cat log stats.txt > log.txt 2>/dev/null
    rm -f log stats.txt pred.txt
# check whether we want to keep the model of this round
    if [ `python ../check_model_save.py config.conf $i` -eq 0 ]
    then
        rm `printf %04d $i`.model
    fi
done

