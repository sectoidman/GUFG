#!/bin/bash
awk '{
	gain = match($0, /Damage/)
	if(gain){
		print $1, $2+$2/2
		if($3) print " " $3, $4+$4/2
		if($5) print " " $5, $6+$6/2
		if($7) print " " $7, $8+$8/2
		if($9) print " " $9, $10+$10/2
		if($11) print " " $11, $12+$12/2
		if($13) print " " $13, $14+$14/2
	} else {
		print $0
	}
}' $1 > $1.dbl

sed ':a;N;$!ba;s/\n\t/\t/g' $1.dbl | sed ':a;N;$!ba;s/\n\ /\ /g' > $1

rm $1.dbl

#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
