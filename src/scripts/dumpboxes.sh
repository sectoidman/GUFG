#!/bin/bash
M="-"
if !([ -x bmpout ]); then
	./scons bmpout
fi
if (expr $(cat src/charlist.h | grep $1 | wc -l) > 0); then
	M=$1
fi
	echo $M
for x in $(cat src/charlist.h | grep $M | cut -d ' ' -f 3); do
	cd content/characters/$x;
	for z in *{,/*}.mv; do
		cd ../../..
		mkdir -p dump/$x;
		./bmpout content/characters/$x/$z;
		cd dump;
		for y in *.bmp; do
			convert -flip $y $x/$y;
			rm $y;
		done
		cd ../content/characters/$x/;
	done
	cd ../../..;
	mkdir -p dump/$x; mv 'content/characters/'$x/*{,/*}bmp dump/$x;
done
