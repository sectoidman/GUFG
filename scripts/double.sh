#!/bin/bash
awk '{
	gain = match($0, /Gain/)
	if(gain){
		print $1, $2, $3*0
	} else {
		print $0
	}
}' $1 > $1.dbl

sed ':a;N;$!ba;s/\n\t/\t/g' $1.dbl | sed ':a;N;$!ba;s/\n\ /\ /g' > $1

rm $1.dbl

#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
