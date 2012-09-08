#!/bin/bash
awk '{
	gain = match($0, /Gain/)
	if(gain){
		print $1, 2*$2
		if($3) print " " $3, 2*$4
		if($5) print " " $5, 2*$6
		if($7) print " " $7, 2*$8
		if($9) print " " $9, 2*$10
		if($11) print " " $11, 2*$12
		if($13) print " " $13, 2*$14
	} else {
		print $0
	}
}' $1 > $1.dbl

sed ':a;N;$!ba;s/\n\t/\t/g' $1.dbl | sed ':a;N;$!ba;s/\n\ /\ /g' > $1

rm $1.dbl

#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
