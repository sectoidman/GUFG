#!/bin/bash
awk '{
	rects = match($0, /\$C/)
	if(rects) {
		print $1"\t" $2, $3, $4, int($5*(270/330))
		if($6 || $7 || $8 || $9)
			print "\t" $6, $7, $8, int($9*(330/300))
		if($10 || $11 || $12 || $13)
			print "\t" $10, $11, $12, int($13*(330/300))
		if($14 || $15 || $16 || $17)
			print "\t" $14, $15, $16, int($17*(330/300))
		if($18 || $19 || $20 || $21) 
			print "\t" $18, $19, $20, int($21*(330/300))
		if($22 || $23 || $24 || $25) 
			print "\t" $22, $23, $24, int($25*(330/300))
	} else {
		print $0
	}
}' $1 > $1.dbl

sed ':a;N;$!ba;s/\n\t/\t/g' $1.dbl | sed ':a;N;$!ba;s/\n\ /\ /g' > $1

rm $1.dbl

#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
