/*
 *  Password verification in Microsoft Word 8.0
 *  see D_CREDITS & D_README
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "wv.h"
#undef S32
#include "rc4.h"
#include "md5.h"

MD5_CTX valContext;
void MD5StoreDigest(MD5_CTX *mdContext);

void makekey(U32 block, rc4_key *key)
	{
	MD5_CTX mdContext;
    U8 pwarray[64];

    memset(pwarray, 0, 64);

    /* 40 bit of hashed password, set by verifypwd() */
    memcpy(pwarray, valContext.digest, 5);

    /* put block number in byte 6...9 */
	pwarray[5] = (U8)(block & 0xFF);         
	pwarray[6] = (U8)((block >>  8) & 0xFF); 
	pwarray[7] = (U8)((block >> 16) & 0xFF); 
	pwarray[8] = (U8)((block >> 24) & 0xFF); 

    pwarray[9] = 0x80;
    pwarray[56] = 0x48;

	MD5Init (&mdContext);
	MD5Update (&mdContext, pwarray, 64);
	MD5StoreDigest(&mdContext);
    prepare_key (mdContext.digest, 16, key);
	}


int verifypwd (U8 pwarray[64], U8 docid[16], U8 salt[64], U8 hashedsalt[16])
	{
	MD5_CTX mdContext1,mdContext2;
    rc4_key key;
    int offset, keyoffset;
	unsigned int tocopy;

	MD5Init (&mdContext1);
	MD5Update (&mdContext1, pwarray, 64);
	MD5StoreDigest(&mdContext1);

    offset = 0;
    keyoffset = 0;
    tocopy = 5;

	MD5Init (&valContext);

    while (offset != 16)
    {
        if ((64 - offset) < 5)
            tocopy = 64 - offset;

        memcpy (pwarray + offset, mdContext1.digest + keyoffset, tocopy);
        offset += tocopy;

        if (offset == 64)
        {
			MD5Update (&valContext, pwarray, 64);
            keyoffset = tocopy;
            tocopy = 5 - tocopy;
            offset = 0;
            continue;
        }

        keyoffset = 0;
        tocopy = 5;
        memcpy (pwarray + offset, docid, 16);
        offset += 16;
    }

    /* Fix (zero) all but first 16 bytes */

    pwarray[16] = 0x80;
    memset (pwarray + 17, 0, 47);
    pwarray[56] = 0x80;
    pwarray[57] = 0x0A;

	MD5Update (&valContext, pwarray, 64);
	MD5StoreDigest(&valContext);

    /* Generate 40-bit RC4 key from 128-bit hashed password */

	makekey(0, &key);
	
    rc4 (salt, 16, &key);
    rc4 (hashedsalt, 16, &key);

    salt[16] = 0x80;
    memset(salt+17, 0, 47);
    salt[56] = 0x80;

	MD5Init (&mdContext2);
	MD5Update (&mdContext2, salt, 64);
	MD5StoreDigest(&mdContext2);

    return (memcmp (mdContext2.digest, hashedsalt, 16));
	}

void expandpw (U16 password[16], U8 pwarray[64])
	{
	/* expandpw expects null terminated 16bit unicode input */
    int i;

    for (i=0; i<64; i++)
    	pwarray[i] =  0;

    i=0;
	while(password[i])
		{
        pwarray[2*i] = (password[i] & 0xff);
        pwarray[(2*i)+1] = ((password[i]>>8)&0xff);
		i++;
		}

    pwarray[2*i] = 0x80;
    pwarray[56] = (i << 4);
	}

#if 0
int main (int argc, char *argv[])
	{
	wvParseStruct ps;
    FILE *outtable,*input,*enc;
	/* max password is 16 unicode chars long */
    U16 password[16] = {'p','a','s','s','w','o','r','d','1',0};
    U8 pwarray[64];
    U8 docid[16], salt[64], hashedsalt[16], x;
    int i,ret=0,j,end;
	unsigned int blk;
    rc4_key key;
	unsigned char test[0x10];
	char *s;
	int len;

	if (argc < 2)
		{
		printf("Usage: ./a.out password wordfile\n");
		return(-1);
		}

	input = fopen(argv[2],"rb");
	ret = wvInitParser(&ps,input);
	wvError(("ret is %d\n",ret));
	if (ret != 4)
        {
        wvError(("file is not anencrypted word97 document %s\n",argv[1]));
        wvOLEFree();
        return(2);
        }

#if 0
    ps.tablefd = fopen ("en.01", "r+b");
#endif

    for (i=0; i<4; i++)
        x = getc (ps.tablefd);

    for (i=0; i<16; i++)
        docid[i] = getc (ps.tablefd);

    for (i=0; i<16; i++)
        salt[i] = getc (ps.tablefd);

    for (i=0; i<16; i++)
        hashedsalt[i] = getc (ps.tablefd);

    if (argc > 1)
		{
		i=0;
		s = argv[1];
		while (*s)
			{
			len = our_mbtowc(&password[i], s, 5);
			i++;
			s+=len;
			}
		}

    expandpw (password, pwarray);

    if (verifypwd (pwarray, docid, salt, hashedsalt))
        printf ("Wrong password\n");
    else
        printf ("Correct password\n");
    /*
    now, what all that previous code does is take in some bytes that word leaves
    around for the express purpose of testing that the password is correct, once
    we've done all that we have the correct key for rc4 in keytable3.
    */

	enc = ps.tablefd;

    fseek(enc,0,SEEK_END);
    end = ftell(enc);

    j = 0;
    fseek(enc,j,SEEK_SET);

	outtable = fopen("tablefd","w+b");

	blk = 0;
    makekey(blk, &key);

    while (j<end)
        {
        for (i=0;i<0x10;i++)
            test[i] = getc(enc);

        rc4(test,0x10,&key);

        for (i=0;i<0x10;i++)
            fputc(test[i],outtable);
        j+=0x10;
		if ((j % 0x200) == 0)
			{
           	/* 
			at this stage we need to rekey the rc4 algorithm
			Dieter Spaar <spaar@mirider.augusta.de> figured out
			this rekeying, big kudos to him 
			*/
            blk++;
            makekey(blk, &key);
	       	}

		}



	enc = ps.mainfd;

    fseek(enc,0,SEEK_END);
    end = ftell(enc);

    j = 0;
    fseek(enc,j,SEEK_SET);

	outtable = tmpfile();

	blk = 0;
    makekey(blk, &key);

    while (j<end)
        {
        for (i=0;i<0x10;i++)
            test[i] = getc(enc);

        rc4(test,0x10,&key);

        for (i=0;i<0x10;i++)
            fputc(test[i],outtable);
        j+=0x10;
		if ((j % 0x200) == 0)
			{
           	/* 
			at this stage we need to rekey the rc4 algorithm
			Dieter Spaar <spaar@mirider.augusta.de> figured out
			this rekeying, big kudos to him 
			*/
            blk++;
            makekey(blk, &key);
	       	}

		}
	
	wvOLEFree();
	fclose(outtable);
	return(ret);
	}
#endif


int wvDecrypt97(wvParseStruct *ps)
	{
    FILE *outtable,*outmain,*enc;
    U8 pwarray[64];
    U8 docid[16], salt[64], hashedsalt[16], x;
    int i,j,end;
	unsigned int blk;
    rc4_key key;
	unsigned char test[0x10];

	for (i=0; i<4; i++)
        x = getc (ps->tablefd);

    for (i=0; i<16; i++)
        docid[i] = getc (ps->tablefd);

    for (i=0; i<16; i++)
        salt[i] = getc (ps->tablefd);

    for (i=0; i<16; i++)
        hashedsalt[i] = getc (ps->tablefd);

    expandpw (ps->password, pwarray);

    if (verifypwd (pwarray, docid, salt, hashedsalt))
        return(1);

	enc = ps->tablefd;

    fseek(enc,0,SEEK_END);
    end = ftell(enc);

    j = 0;
    fseek(enc,j,SEEK_SET);

	outtable = tmpfile();

	blk = 0;
    makekey(blk, &key);

    while (j<end)
        {
        for (i=0;i<0x10;i++)
            test[i] = getc(enc);

        rc4(test,0x10,&key);

        for (i=0;i<0x10;i++)
            fputc(test[i],outtable);
        j+=0x10;
		if ((j % 0x200) == 0)
			{
           	/* 
			at this stage we need to rekey the rc4 algorithm
			Dieter Spaar <spaar@mirider.augusta.de> figured out
			this rekeying, big kudos to him 
			*/
            blk++;
            makekey(blk, &key);
	       	}

		}

	enc = ps->mainfd;

    fseek(enc,0,SEEK_END);
    end = ftell(enc);

    j = 0;
    fseek(enc,j,SEEK_SET);

	outmain = fopen("mainfd","w+b");

	blk = 0;
    makekey(blk, &key);

    while (j<end)
        {
        for (i=0;i<0x10;i++)
            test[i] = getc(enc);

        rc4(test,0x10,&key);

        for (i=0;i<0x10;i++)
            fputc(test[i],outmain);
        j+=0x10;
		if ((j % 0x200) == 0)
			{
           	/* 
			at this stage we need to rekey the rc4 algorithm
			Dieter Spaar <spaar@mirider.augusta.de> figured out
			this rekeying, big kudos to him 
			*/
            blk++;
            makekey(blk, &key);
	       	}

		}

	if (ps->tablefd0)
		fclose(ps->tablefd0);
	if (ps->tablefd1)
		fclose(ps->tablefd1);
	fclose(ps->mainfd);
	ps->tablefd = outtable;
	ps->tablefd0 = outtable;
	ps->tablefd1 = outtable;
	ps->mainfd = outmain;
	rewind(ps->tablefd);
	rewind(ps->mainfd);
	ps->fib.fEncrypted=0;
	wvGetFIB(&ps->fib,ps->mainfd);
	ps->fib.fEncrypted=0;
	return(0);
	}


/* 
this is just cut out of MD5Final to get the byte order correct
under MSB systems, the previous code was woefully tied to intel
x86

C.
*/
void MD5StoreDigest(MD5_CTX *mdContext)
	{
	unsigned int i, ii;
	/* store buffer in digest */
	for (i = 0, ii = 0; i < 4; i++, ii += 4) 
		{
		mdContext->digest[ii] = (unsigned char)(mdContext->buf[i] & 0xFF);
		mdContext->digest[ii+1] =
		(unsigned char)((mdContext->buf[i] >> 8) & 0xFF);
		mdContext->digest[ii+2] =
		(unsigned char)((mdContext->buf[i] >> 16) & 0xFF);
		mdContext->digest[ii+3] =
		(unsigned char)((mdContext->buf[i] >> 24) & 0xFF);
		}
	}
