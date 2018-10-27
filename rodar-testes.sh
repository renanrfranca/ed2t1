#!/bin/bash
for i in {1..30}
do
    ./main <casos-teste/${i}.in >casos-teste/${i}.outa
    echo "================== diff do teste ${i} ======================"
    diff casos-teste/${i}.out casos-teste/${i}.outa
done