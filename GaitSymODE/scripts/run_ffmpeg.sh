#!/bin/sh

ffmpeg -i Smooth_Frame%05d.png -sameq -r 25 -vcodec mpeg1video movie.mpg
