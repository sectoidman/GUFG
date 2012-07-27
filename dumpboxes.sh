#!/bin/bash
for x in $(cat charlist.h | grep - | cut -d   -f 3); do ./bmpout $x/*mv; mkdir -p dump/$x; mv $x/*bmp dump/$x; done;
