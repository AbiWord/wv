#!/bin/sh

echo "make_epses:"
for i in ${1}*.wmf ; do
  STEM=`basename $i wmf`
  echo "converting $STEM"wmf
  wmftoeps $i ${STEM}eps
done

for j in png jpg jpeg ; do
  for i in ${1}*.${j} ; do
    STEM=`basename $i $j`
    if [ -f $i ] ; then 
      echo "converting $STEM"$j
      convert $i ${STEM}eps
    fi
  done
done
