#include <assert.h>
#include <stdio.h>
#include "zlib.h"

int main(int argc, char *argv[])
{
  char *compr;
  char uncompr[99999];
  FILE* fp;
  int err;
  uLong uncomprLen, comprLen;

  fp = fopen("graphic.wmf.lzzed", "r");
  assert(fp);

  /* Figure out the size of the file in a boring way*/
  assert(fseek(fp, 0, SEEK_END) == 0);
  comprLen = ftell(fp);
  assert(fseek(fp, 0, SEEK_SET) == 0);

  /* Read in the file contents */
  compr = malloc(comprLen);
  fread(compr, comprLen, 1, fp);
  fclose(fp);

  uncomprLen = sizeof(uncompr);	/* This was the trick :( */
  err = uncompress(uncompr, &uncomprLen, compr, comprLen);

  if (err != Z_OK) {
    fprintf(stderr, "error: %d\n", err); 
    exit(1); 
  } 

  /* Write out uncompressed data */
  fp = fopen("t.t", "w");
  fwrite(uncompr, uncomprLen, 1, fp);
  fclose(fp);
  
  return 0;
}
