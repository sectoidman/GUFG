#!/bin/bash
counter=0
awk '{
	comment = match($0, /\#/)
	if(comment) {
		if(!counter) counter=0
		print "#"counter
		counter += 1
	} else {
		print $0
	}
}' $1 > $1.dbl
sed ':a;N;$!ba;s/\n\t/\t/g' $1.dbl | sed ':a;N;$!ba;s/\n\ /\ /g' > $1
rm $1.dbl

#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
#	awk '/\$\D*\t/ {printf "%s\n", substr($1,0,10)}' $1
