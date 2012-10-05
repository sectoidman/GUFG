#!/bin/bash
for i in $(cat .gitignore)
do
	rm -rv $i
done
