void wvPutSEPX(version ver,SEPX *item,wvStream *fd)
         {
         U16 i = (U16)0;

		 write_16ubit(fd. item->cb);

         if (!item->cb)
         	return;

         for (i=0;i<item->cb;i++)
                 {
                 write_8ubit(fd, item->grpprl[i]);
                 }
         }