/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%       TTTTT  RRRR    AAA   N   N  SSSSS  FFFFF   OOO   RRRR   M   M         %
%         T    R   R  A   A  NN  N  SS     F      O   O  R   R  MM MM         %
%         T    RRRR   AAAAA  N N N   SSS   FFF    O   O  RRRR   M M M         %
%         T    R R    A   A  N  NN     SS  F      O   O  R R    M   M         %
%         T    R  R   A   A  N   N  SSSSS  F       OOO   R  R   M   M         %
%                                                                             %
%                                                                             %
%                   ImageMagick Image Transform Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright 1999 E. I. du Pont de Nemours and Company                        %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  E. I. du Pont de Nemours and Company be liable for any claim, damages or   %
%  other liability, whether in an action of contract, tort or otherwise,      %
%  arising from, out of or in connection with ImageMagick or the use or other %
%  dealings in ImageMagick.                                                   %
%                                                                             %
%  Except as contained in this notice, the name of the E. I. du Pont de       %
%  Nemours and Company shall not be used in advertising or otherwise to       %
%  promote the sale, use or other dealings in ImageMagick without prior       %
%  written authorization from the E. I. du Pont de Nemours and Company.       %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l i p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FlipImage creates a new image that reflects each scanline in the
%  vertical direction It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FlipImage method is:
%
%      Image *FlipImage(const Image *image)
%
%  A description of each parameter follows:
%
%    o flipped_image: Method FlipImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export Image *FlipImage(const Image *image)
{
#define FlipImageText  "  Flipping image...  "

  Image
    *flipped_image;

  int
    y;

  register int
    runlength,
    x;

  register RunlengthPacket
    *p,
    *q,
    *s;

  RunlengthPacket
    *scanline;

  /*
    Initialize flipped image attributes.
  */
  assert(image != (Image *) NULL);
  flipped_image=CloneImage(image,image->columns,image->rows,False);
  if (flipped_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to flip image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(RunlengthPacket *)
    AllocateMemory(image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to reflect image",
        "Memory allocation failed");
      DestroyImage(flipped_image);
      return((Image *) NULL);
    }
  /*
    Flip each row.
  */
  p=image->pixels;
  runlength=p->length+1;
  q=flipped_image->pixels+flipped_image->packets-1;
  for (y=0; y < (int) flipped_image->rows; y++)
  {
    /*
      Read a scan line.
    */
    s=scanline;
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Flip each column.
    */
    s=scanline+image->columns;
    for (x=0; x < (int) flipped_image->columns; x++)
    {
      s--;
      *q=(*s);
      q->length=0;
      q--;
    }
  }
  FreeMemory((char *) scanline);
  return(flipped_image);
}
