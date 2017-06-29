#!/bin/bash

cat $1 | awk 'BEGIN {counter = 0; x = 1;} {if ($1 ~ "iprec") {  printf("%d, %s\n", x, $2); if (++counter == 11) { counter = 0; x++; } }  }'
