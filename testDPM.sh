#!/bin/bash

if [[ ("$#" -lt 3) || ($1 -lt 7) ]]; then
   echo "usage: min max repeat seed dry verifier_only securityparam"
   exit
fi

function calcn {
	len=$(( (2**(i-1))/62 ))
}

if [ "$5" = 0 ]; then
   dirname=~/testlogs/$(date +%y%m%d-%H%M%S)DPM
   mkdir -p $dirname
fi

cmd="./bin/starkdpm/stark-dpm "
seed="$4"

for ((i=$1; i<=$2; i++))
do
   calcn
for ((j=1; j<=$3; j++))
do
   if [[ $4 -eq 0 ]]; then
       seed=$RANDOM
   fi
   args="""${len} ${seed} ${6} ${7}"""
   echo "repetition j=${j}, t=${i}: " $cmd "${args}"
   if [ "$5" = 0 ]; then
      echo "args:" ${args} > $dirname/DPM-t${i}L${len}-00${j}.txt
      perf stat $cmd ${args} 2>&1 | tee -a $dirname/DPM-t${i}L${len}-00${j}.txt
   fi
done
done
