#!/bin/bash

securityList="60 80 120"

function getSeconds {
num=`echo $1 | cut -d" " -f1`
units=`echo $1 | cut -d" " -f2`
if [ "$units" == "Seconds" ]; then 
echo $num
elif [ "$units" == "Minutes" ]; then
echo "${num}*60" | bc
elif [ "$units" == "Hours" ]; then
echo "${num}*60*60" | bc
elif [ "$units" == "Days" ]; then
echo "${num}*60*60*24" | bc
else
echo "NA"
fi
}

function getBytes {
num=`echo $1 | cut -d" " -f1`
units=`echo $1 | cut -d" " -f2`
if [ "$units" == "Bytes" ]; then 
echo $num
elif [ "$units" == "KBytes" ]; then
echo "${num}*1024" | bc | cut -d. -f1
elif [ "$units" == "MBytes" ]; then
echo "${num}*1024*1024" | bc| cut -d. -f1
elif [ "$units" == "GBytes" ]; then
echo "${num}*1024*1024*1024" | bc| cut -d. -f1
elif [ "$units" == "TBytes" ]; then
echo "${num}*1024*1024*1024*1024" | bc| cut -d. -f1
elif [ "$units" == "PBytes" ]; then
echo "${num}*1024*1024*1024*1024*1024" | bc| cut -d. -f1
else
echo "NA"
fi
}

printf "t,"
for security in ${securityList}
do
printf "prover seconds (security ${security} bits),"
printf "verifier seconds (security ${security} bits),"
printf "argument bytes (security ${security} bits),"
done

printf "\n"

for t in {1..24}
do
printf "${t},"
for security in ${securityList}
do
filename="subsetsum-t${t}-s${security}.txt"
ptime=`grep "Prover time" ${filename} | cut -d= -f2 | cut -d\| -f1`
vtime=`grep "Verifier time" ${filename} | cut -d= -f2 | cut -d\| -f1`
arglen=`grep "Total communication complexity" ${filename} | cut -d= -f2 | cut -d\| -f1`
printf "$(getSeconds "$ptime"),"
printf "$(getSeconds "$vtime"),"
printf "$(getBytes "$arglen"),"
done
printf "\n"
done
