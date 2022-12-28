#!/bin/bash
filename=$1
tr -cs A-Za-z '\n' < $filename | tr A-Z a-z | sort | uniq -c | sort -rn | head -1 | tr -d ' ' | tr -d '[:digit:]'
#tr -c '[:alnum:]' '[\n*]' < $filename | sort | uniq -c | sort -nr | head  -5
#sed -e 's/\s/\n/g' < $filename | sort | uniq -c | sort -nr | head  -1

