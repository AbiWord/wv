/*
 *  Password verification in Microsoft Word 8.0
 *  see D_CREDITS & D_README
 */

#include <stdio.h>
#include <sys/types.h>
#include "wv.h"
#include "rc4.h"

u_long keytab3[4];
u_long keytab2[4];

void MD5Init (u_long buf[4])
{
    buf[0] = 0x67452301;
    buf[1] = 0xefcdab89;
    buf[2] = 0x98badcfe;
    buf[3] = 0x10325476;
}

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

#define MD5STEP(f, w, x, y, z, data, s) \
    ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

void MD5Transform (u_long buf[4], u_long in[16])
{
    register u_long a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

void makekey(u_long block, rc4_key *key)
{
    u_long keytab3[4];
    u_char pwarray[64];

    MD5Init (keytab3);
    memset(pwarray, 0, 64);

    /* 40 bit of hashed password, set by verifypwd() */

    memcpy(pwarray, keytab2, 5);

    /* put block number in byte 6...9 */

	pwarray[5] = (u_char)(block & 0xFF);         /* & 0xFF not reall needed */
	pwarray[6] = (u_char)((block >>  8) & 0xFF); /* & 0xFF not reall needed */
	pwarray[7] = (u_char)((block >> 16) & 0xFF); /* & 0xFF not reall needed */
	pwarray[8] = (u_char)((block >> 24) & 0xFF); /* & 0xFF not reall needed */

    pwarray[9] = 0x80;
    pwarray[56] = 0x48;
    MD5Transform (keytab3, (u_long *) pwarray);

    prepare_key ((u_char *) &keytab3, 16, key);
}


int verifypwd (u_char pwarray[64], u_char docid[16], u_char salt[64], u_char hashedsalt[16])
{
    rc4_key key;
    u_long keytab1[4], keytab4[4];
    int offset, keyoffset, tocopy;

    MD5Init (keytab1);
    MD5Transform (keytab1, (u_long *) pwarray);

    offset = 0;
    keyoffset = 0;
    tocopy = 5;

    MD5Init (keytab2);

    while (offset != 16)
    {
        if ((64 - offset) < 5)
            tocopy = 64 - offset;

        memcpy (pwarray + offset, (char *) keytab1 + keyoffset, tocopy);
        offset += tocopy;

        if (offset == 64)
        {
            MD5Transform (keytab2, (u_long *) pwarray);
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
    MD5Transform (keytab2, (u_long *) pwarray);

    /* Generate 40-bit RC4 key from 128-bit hashed password */

	makekey(0, &key);

    rc4 (salt, 16, &key);
    rc4 (hashedsalt, 16, &key);

    MD5Init (keytab4);
    salt[16] = 0x80;
    memset(salt+17, 0, 47);
    salt[56] = 0x80;
    MD5Transform (keytab4, (u_long *) salt);

    return (memcmp (keytab4, hashedsalt, 16));
}

void expandpw (u_char password[16], u_char pwarray[64])
{
    int i;

    for (i=0; i<64; i++)
        pwarray[i] = 0;

    for (i=0; password[i]; i++)
        pwarray[2*i] = password[i];

    pwarray[2*i] = 0x80;
    pwarray[56] = (i << 4);
}

#if 0

int main (int argc, char *argv[])
{
	wvParseStruct ps;
    FILE *outtable,*input,*enc;
    u_char password[16];
    u_char pwarray[64];
    u_char docid[16], salt[64], hashedsalt[16], x;
    int i,ret=0,j,end,blk;
    rc4_key key;
	unsigned char test[0x10];

	if (argc < 2)
		{
		printf("Usage: ./a.out password wordfile\n");
		return(-1);
		}

	input = fopen(argv[2],"rb");
	ret = wvInitParser(&ps,input);
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
        strcpy (password, argv[1]);
    else
        strcpy (password, "password1");
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

	outtable = fopen("mainfd","w+b");

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
    u_char password[16];
    u_char pwarray[64];
    u_char docid[16], salt[64], hashedsalt[16], x;
    int i,ret=0,j,end,blk;
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

	fclose(ps->tablefd0);
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
