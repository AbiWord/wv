/* RC4 Encrypt/Decrypt

   RC4 was developped by RSA labs, however this code may not be exact.

   Written in portable Micro-C by Tom St Denis.

   Notes:

        1.  I found the 'source' on a discussion website.  The user
        posting confirms that this matches the BSAFE2 algorithm.

        2.  The user key can be upto 256 bytes in length.

        3.  Yes the same algorithm is used for decryption.

        4.  I have optimized the code from the original post to work
        well without the modulus.  Also I use 16-bit pointers in the
        arrays since some C compilers don't handle char indexes well.

        5.  Originally RC4 was a stream cipher, but for reasons of speed
        it is a variable block cipher here.  You pass it the address of
        the buffer and length as the first to parameters to rc4() and the
        rc4_key as the second parameter.

        6.  As a side note this cipher runs at about 84.7KB/s on my
        AMD486 40mhz.


   Micro-C is Copyrighted Dave Dunfield.
   Tom St Denis, 1999
*/

/* a RC4 expanded key session */
struct rc4_key {
    unsigned char state[256];
    unsigned x, y;
};

/* expand a key (makes a rc4_key) */
void prepare_key(unsigned char *keydata, unsigned len, struct rc4_key *key)
{
    unsigned index1, index2, counter;
    unsigned char *state;

    state = key->state;

    for (counter = 0; counter < 256; counter++)
        state[counter] = counter;

    key->x = key->y = index1 = index2 = 0;

    for (counter = 0; counter < 256; counter++) {
        index2 = (keydata[index1] + state[counter] + index2) & 255;

        /* swap */
        state[counter] ^= state[index2];
        state[index2]  ^= state[counter];
        state[counter] ^= state[index2];

        index1 = (index1 + 1) % len;
    }
}

/* reversible encryption, will encode a buffer updating the key */
void rc4(unsigned char *buffer, unsigned len, struct rc4_key *key)
{
    unsigned x, y, xorIndex, counter;
    unsigned char *state;

    /* get local copies */
    x = key->x; y = key->y;
    state = key->state;

    for (counter = 0; counter < len; counter++) {
        x = (x + 1) & 255;
        y = (state[x] + y) & 255;

        /* swap */
        state[x] ^= state[y];
        state[y] ^= state[x];
        state[x] ^= state[y];

        xorIndex = (state[y] + state[x]) & 255;

        buffer[counter] ^= state[xorIndex];
    }

    key->x = x; key->y = y;
}


/* test */
#include <stdio.h>

/* user key, upto 256 bytes */
unsigned char user_key[8];          /* 8 bytes in our demo */
struct rc4_key key;                 /* key used during encryption/decryption */

unsigned char in[4], out[4], out2[4]; /* test data */

int main(void)
{
    /* test vector */
    memset(in, 2, sizeof(in));

    /* encrypt */
    memset(user_key, 0, sizeof(user_key));          /* build sesion key */
    prepare_key(user_key, sizeof(user_key), &key);
    memcpy(out, in, sizeof(in));
    rc4(out, 4, &key);

    /* decrypt */
    memset(user_key, 0, sizeof(user_key));          /* build session key */
    prepare_key(user_key, sizeof(user_key), &key);
    memcpy(out2, out, sizeof(out));
    rc4(out2, 4, &key);

    /* output */
    printf("Normal : %2x %2x %2x %2x\n", in[0], in[1], in[2], in[3]);
    printf("Encoded: %2x %2x %2x %2x\n", out[0], out[1], out[2], out[3]);
    printf("Decoded: %2x %2x %2x %2x\n", out2[0], out2[1], out2[2], out2[3]);
}
