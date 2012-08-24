#!/bin/bash
if !([ -x bmpout ]); then
	make bmpout
fi
for x in $(cat charlist.h | grep - | cut -d ' ' -f 3); do 
	./bmpout $x/*.mv;
	cd $x;
	for y in $(ls *bmp); do
		convert -flip $y 'flip'$y;
		mv 'flip'$y $y
	done
	cd ..;
	mkdir -p dump/$x; mv $x/*bmp dump/$x; 
done
