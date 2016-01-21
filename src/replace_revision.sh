#!/bin/sh

if [ -e "$2" ]; then
rev_in=`grep "GIT_COMMIT" $2 | awk '{print $3}' | tr -d '"'`
else
rev_in=""
fi
rev=`git rev-parse HEAD`;

if [ -n "$3" ] || [ "$rev" != "$rev_in" ]; then
  mkdir -p `dirname $2`
  sed -E "s/#define[[:space:]]+GIT_COMMIT[[:space:]].*/#define GIT_COMMIT \"$rev\"/" $1 > $2
fi
