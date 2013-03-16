#!/bin/bash
M="-"
if !([ -x bmpout* ]); then
	scons bmpout
fi
if (expr $(cat src/charlist.h | grep $1 | wc -l) > 0); then
	M=$1
fi
	echo $M
for x in $(cat src/charlist.h | grep $M | cut -d ' ' -f 3); do
	for z in $(ls content/characters/$x | grep .mv); do ./bmpout* $x/$z; done
	cd 'content/characters/'$x;
	for y in $(ls | grep .bmp); do
		convert -flip $y 'flip'$y;
		mv 'flip'$y $y
	done
	cd ../../..;
	mkdir -p dump/$x; mv 'content/characters/'$x/*bmp dump/$x;
done
