#!/bin/sh

str=$3
if [ -e "$2" ]; then
rev_in=`grep "$3" $2 | awk '{print $3}' | tr -d '"'`
else
rev_in=""
fi
rev=`git rev-parse HEAD`;

if [ -n "$4" ] || [ "$rev" != "$rev_in" ]; then
  mkdir -p `dirname $2`
  sed -E "s/#define[[:space:]]+$3[[:space:]].*/#define $3 \"$rev\"/" $1 > $2
fi
