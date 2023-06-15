#!/bin/sh

width=800
height=600

for i
do
    povray +I $i +W$width +H$height +Q9 -J0.0 +A0.05 +AM1 -D
done

