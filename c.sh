#!/bin/bash
filename=$1
a=$(tr -d "[:space:]" < $filename)
echo $a > $filename
cat $filename
# test comment