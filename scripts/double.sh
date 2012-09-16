#!/bin/bash
awk '{
	gain = match($0, /\$C/)
	if(gain){
		print $1"\t"$2, $3*0, $4, $5
	} else {
		r = match($0, /\$R/)
		if(r){
			print $1"\t"$2, $3*0, $4*0, $5*0
		}
		else print $0
	}
}' $1 > $1.dbl

sed ':a;N;$!ba;s/\n\t/\t/g' $1.dbl | sed ':a;N;$!ba;s/\n\ /\ /g' > $1

rm $1.dbl

#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
