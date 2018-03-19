#!/bin/bash

for t in {1..24}
do
    for security in {60,80,120}
    do
        cmd="./stark-tinyram examples-tinyram/coNP-subsetsum-noRAM.asm -t${t} -s${security}"
        echo $cmd
        $cmd > subsetsum-t${t}-s${security}.txt
    done
done
