#!/bin/sh

for i in *.wmf ; do
  STEM=`basename $i wmf`
  wmftoeps $i ${STEM}eps
done

for j in png jpg jpeg ; do
  for i in *.${j} ; do
    STEM=`basename $i $j`
    if [ -f $i ] ; then 
      convert $i ${STEM}eps
    fi
  done
done
