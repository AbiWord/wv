#!/bin/sh
sed -e "s/ushort /U16 /" -e "s/ulong /U32 /" -e "s/uchar /U8 /" -e "s/short /S16 /" -e "s/long /S32 /" -e "s/char /S8 /"
