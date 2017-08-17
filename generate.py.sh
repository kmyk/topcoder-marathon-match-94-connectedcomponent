#!/bin/bash
read sq_s
echo $sq_s >&2
head -n $sq_s >&2
s=$(echo $sq_s | dc -e '?vp')
echo $s
seq 0 $(( $s - 1 ))
