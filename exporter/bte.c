/* again, no PC_LF versions. sorry. */


void wvPutBTE(BTE *bte,wvStream *fd)
{
       	U32 temp32 = (U32)0;

        /* bte->pn = temp32 & 0x003fffffL;
           bte->unused = (temp32 & 0xffc00000L)>>22;
         */

		temp32 |= bte->pn;
		temp32 |= unused << 22;

		write_32ubit(fd, temp32);
}