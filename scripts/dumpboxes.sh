#!/bin/bash
M="-"
if !([ -x bmpout ]); then
	make bmpout
fi
if (expr $(cat src/charlist.h | grep $1 | wc -l) > 0); then
	M=$1
fi
	echo $M
for x in $(cat src/charlist.h | grep $M | cut -d ' ' -f 3); do 
	./bmpout $x/*.mv;
	cd $x;
	for y in $(ls *bmp); do
		convert -flip $y 'flip'$y;
		mv 'flip'$y $y
	done
	cd ..;
	mkdir -p dump/$x; mv $x/*bmp dump/$x; 
done
