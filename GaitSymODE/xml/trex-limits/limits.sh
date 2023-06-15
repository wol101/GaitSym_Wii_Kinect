#!/bin/sh

list="Ankle Knee Hip"

for i in $list
do
    echo "$i"
    ./max_param.py "$i.txt" Angle
done
for i in $list
do
    echo "${i}Flexor"
    ./max_param.py "${i}Flexor.txt" m_Length
    echo "${i}Extensor"
    ./max_param.py "${i}Extensor.txt" m_Length
done

