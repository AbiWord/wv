/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     IIIII  M   M   AAA    GGGG  EEEEE                       %
%                       I    MM MM  A   A  G      E                           %
%                       I    M M M  AAAAA  G  GG  EEE                         %
%                       I    M   M  A   A  G   G  E                           %
%                     IIIII  M   M  A   A   GGGG  EEEEE                       %
%                                                                             %
%                                                                             %
%                          ImageMagick Image Methods                          %
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
  Constant declaration.
*/
const char
  *DefaultPointSize = "12",
  *DefaultTileFrame = "15x15+3+3",
  *DefaultTileLabel = "%f\n%wx%h\n%b",
  *DefaultTileGeometry = "106x106+4+3>",
  *LoadImageText = "  Loading image...  ",
  *LoadImagesText = "  Loading images...  ",
  *ReadBinaryType = "rb",
  *ReadBinaryUnbufferedType = "rbu",
  *SaveImageText = "  Saving image...  ",
  *SaveImagesText = "  Saving images...  ",
  *WriteBinaryType = "wb";

const char
  *BackgroundColor = "#bdbdbd",  /* gray */
  *BorderColor = "#bdbdbd",  /* gray */
  *ForegroundColor = "#000",  /* black */
  *MatteColor = "#bdbdbd";  /* gray */

const InterlaceType
  DefaultInterlace = NoInterlace;


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageInfo initializes the ImageInfo structure.
%
%  The format of the GetImageInfo method is:
%
%      void GetImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export void GetImageInfo(ImageInfo *image_info)
{
  /*
    File and image dimension members.
  */
  GetBlobInfo(&(image_info->blob));
  image_info->file=(FILE *) NULL;
  *image_info->filename='\0';
  *image_info->magick='\0';
  TemporaryFilename(image_info->unique);
  (void) strcat(image_info->unique,"u");
  TemporaryFilename(image_info->zero);
  image_info->affirm=False;
  image_info->temporary=False;
  image_info->adjoin=True;
  image_info->subimage=0;
  image_info->subrange=0;
  image_info->ping=False;
  image_info->depth=QuantumDepth;
  image_info->size=(char *) NULL;
  image_info->tile=(char *) NULL;
  image_info->page=(char *) NULL;
  image_info->interlace=DefaultInterlace;
  image_info->units=UndefinedResolution;
  /*
    Compression members.
  */
  image_info->compression=UndefinedCompression;
  image_info->quality=75;
  /*
    Annotation members.
  */
  image_info->server_name=(char *) NULL;
  image_info->box=(char *) NULL;
  image_info->font=(char *) NULL;
  image_info->pen=(char *) NULL;
  image_info->texture=(char *) NULL;
  image_info->density=(char *) NULL;
  image_info->linewidth=1;
  image_info->pointsize=atoi(DefaultPointSize);
  image_info->antialias=True;
  image_info->fuzz=0;
  image_info->background_color=(char *) NULL;
  image_info->border_color=(char *) NULL;
  image_info->matte_color=(char *) NULL;
  /*
    Color reduction members.
  */
  image_info->dither=True;
  image_info->monochrome=False;
  image_info->colorspace=UndefinedColorspace;
  /*
    Animation members.
  */
  image_info->dispose=(char *) NULL;
  image_info->delay=(char *) NULL;
  image_info->iterations=(char *) NULL;
  image_info->coalesce_frames=False;
  image_info->insert_backdrops=False;
  /*
    Miscellaneous members.
  */
  image_info->verbose=False;
  image_info->preview_type=JPEGPreview;
  image_info->view=(char *) NULL;
  image_info->group=0L;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A l l o c a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AllocateImage allocates an Image structure and initializes each
%  field to a default value.
%
%  The format of the AllocateImage method is:
%
%      Image *AllocateImage(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o allocated_image: Method AllocateImage returns a pointer to an image
%      structure initialized to default values.  A null image is returned if
%      there is a memory shortage.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export Image *AllocateImage(const ImageInfo *image_info)
{
  ColorPacket
    color;

  Image
    *allocated_image;

  int
    flags;

  /*
    Allocate image structure.
  */
  allocated_image=(Image *) AllocateMemory(sizeof(Image));
  if (allocated_image == (Image *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to allocate image",
        "Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Initialize Image structure.
  */
  GetBlobInfo(&(allocated_image->blob));
  allocated_image->file=(FILE *) NULL;
  allocated_image->exempt=False;
  allocated_image->status=False;
  allocated_image->temporary=False;
  *allocated_image->filename='\0';
  allocated_image->filesize=0;
  allocated_image->pipe=False;
  (void) strcpy(allocated_image->magick,"MIFF");
  allocated_image->comments=(char *) NULL;
  allocated_image->label=(char *) NULL;
  allocated_image->class=DirectClass;
  allocated_image->matte=False;
  allocated_image->compression=RunlengthEncodedCompression;
  allocated_image->columns=0;
  allocated_image->rows=0;
  allocated_image->depth=QuantumDepth;
  allocated_image->tile_info.width=0;
  allocated_image->tile_info.height=0;
  allocated_image->tile_info.x=0;
  allocated_image->tile_info.y=0;
  allocated_image->offset=0;
  allocated_image->interlace=DefaultInterlace;
  allocated_image->scene=0;
  allocated_image->units=UndefinedResolution;
  allocated_image->x_resolution=0.0;
  allocated_image->y_resolution=0.0;
  allocated_image->montage=(char *) NULL;
  allocated_image->directory=(char *) NULL;
  allocated_image->colormap=(ColorPacket *) NULL;
  allocated_image->colors=0;
  allocated_image->colorspace=RGBColorspace;
  allocated_image->rendering_intent=UndefinedIntent;
  allocated_image->gamma=0.0;
  allocated_image->chromaticity.red_primary.x=0.0;
  allocated_image->chromaticity.red_primary.y=0.0;
  allocated_image->chromaticity.green_primary.x=0.0;
  allocated_image->chromaticity.green_primary.y=0.0;
  allocated_image->chromaticity.blue_primary.x=0.0;
  allocated_image->chromaticity.blue_primary.y=0.0;
  allocated_image->chromaticity.white_point.x=0.0;
  allocated_image->chromaticity.white_point.y=0.0;
  allocated_image->color_profile.length=0;
  allocated_image->color_profile.info=(unsigned char *) NULL;
  allocated_image->iptc_profile.length=0;
  allocated_image->iptc_profile.info=(unsigned char *) NULL;
  allocated_image->pixels=(RunlengthPacket *) NULL;
  allocated_image->packets=0;
  allocated_image->packet_size=0;
  allocated_image->packed_pixels=(unsigned char *) NULL;
  allocated_image->geometry=(char *) NULL;
  allocated_image->page=(char *) NULL;
  allocated_image->dispose=0;
  allocated_image->delay=0;
  allocated_image->iterations=1;
  allocated_image->fuzz=0;
  allocated_image->filter=LanczosFilter;
  allocated_image->blur=1.0;
  (void) QueryColorDatabase(BackgroundColor,&color);
  allocated_image->background_color.red=XDownScale(color.red);
  allocated_image->background_color.green=XDownScale(color.green);
  allocated_image->background_color.blue=XDownScale(color.blue);
  allocated_image->background_color.index=Opaque;
  (void) QueryColorDatabase(BorderColor,&color);
  allocated_image->border_color.red=XDownScale(color.red);
  allocated_image->border_color.green=XDownScale(color.green);
  allocated_image->border_color.blue=XDownScale(color.blue);
  allocated_image->border_color.index=Opaque;
  (void) QueryColorDatabase(MatteColor,&color);
  allocated_image->matte_color.red=XDownScale(color.red);
  allocated_image->matte_color.green=XDownScale(color.green);
  allocated_image->matte_color.blue=XDownScale(color.blue);
  allocated_image->matte_color.index=Opaque;
  allocated_image->total_colors=0;
  allocated_image->normalized_mean_error=0.0;
  allocated_image->normalized_maximum_error=0.0;
  allocated_image->mean_error_per_pixel=0;
  allocated_image->signature=(char *) NULL;
  *allocated_image->magick_filename='\0';
  allocated_image->magick_columns=0;
  allocated_image->magick_rows=0;
  allocated_image->magick_time=time((time_t *) NULL);
  allocated_image->tainted=False;
  allocated_image->orphan=False;
  allocated_image->previous=(Image *) NULL;
  allocated_image->list=(Image *) NULL;
  allocated_image->next=(Image *) NULL;
  allocated_image->restart_animation_here=False;
  if (image_info == (ImageInfo *) NULL)
    return(allocated_image);
  /*
    Transfer image info.
  */
  allocated_image->blob=image_info->blob;
  (void) strcpy(allocated_image->filename,image_info->filename);
  (void) strcpy(allocated_image->magick_filename,image_info->filename);
  (void) strcpy(allocated_image->magick,image_info->magick);
  if (image_info->size != (char *) NULL)
    {
      int
        y;

      (void) sscanf(image_info->size,"%ux%u",
        &allocated_image->columns,&allocated_image->rows);
      flags=ParseGeometry(image_info->size,&allocated_image->offset,&y,
        &allocated_image->columns,&allocated_image->rows);
      if ((flags & HeightValue) == 0)
        allocated_image->rows=allocated_image->columns;
      allocated_image->tile_info.width=allocated_image->columns;
      allocated_image->tile_info.height=allocated_image->rows;
    }
  if (image_info->tile != (char *) NULL)
    if (!IsSubimage(image_info->tile,False))
      {
        (void) sscanf(image_info->tile,"%ux%u",
          &allocated_image->columns,&allocated_image->rows);
        flags=ParseGeometry(image_info->tile,&allocated_image->tile_info.x,
          &allocated_image->tile_info.y,&allocated_image->columns,
          &allocated_image->rows);
        if ((flags & HeightValue) == 0)
          allocated_image->rows=allocated_image->columns;
      }
  allocated_image->compression=image_info->compression;
  allocated_image->interlace=image_info->interlace;
  allocated_image->units=image_info->units;
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(image_info->density,"%lfx%lf",&allocated_image->x_resolution,
        &allocated_image->y_resolution);
      if (count != 2)
        allocated_image->y_resolution=allocated_image->x_resolution;
    }
  if (image_info->page != (char *) NULL)
    allocated_image->page=PostscriptGeometry(image_info->page);
  if (image_info->dispose != (char *) NULL)
    allocated_image->dispose=atoi(image_info->dispose);
  if (image_info->delay != (char *) NULL)
    allocated_image->delay=atoi(image_info->delay);
  if (image_info->iterations != (char *) NULL)
    allocated_image->iterations=atoi(image_info->iterations);
  allocated_image->depth=image_info->depth;
  if (image_info->background_color != (char *) NULL)
    {
      (void) QueryColorDatabase(image_info->background_color,&color);
      allocated_image->background_color.red=XDownScale(color.red);
      allocated_image->background_color.green=XDownScale(color.green);
      allocated_image->background_color.blue=XDownScale(color.blue);
    }
  if (image_info->border_color != (char *) NULL)
    {
      (void) QueryColorDatabase(image_info->border_color,&color);
      allocated_image->border_color.red=XDownScale(color.red);
      allocated_image->border_color.green=XDownScale(color.green);
      allocated_image->border_color.blue=XDownScale(color.blue);
    }
  if (image_info->matte_color != (char *) NULL)
    {
      (void) QueryColorDatabase(image_info->matte_color,&color);
      allocated_image->matte_color.red=XDownScale(color.red);
      allocated_image->matte_color.green=XDownScale(color.green);
      allocated_image->matte_color.blue=XDownScale(color.blue);
    }
  return(allocated_image);
}

Export unsigned int IsSubimage(const char *geometry,const unsigned int pedantic)
{
  int
    x,
    y;

  unsigned int
    flags,
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=ParseGeometry((char *) geometry,&x,&y,&width,&height);
  if (pedantic)
    return((flags != NoValue) && !(flags & HeightValue));
  return(IsGeometry(geometry) && !(flags & HeightValue));
}


Export unsigned int IsGeometry(const char *geometry)
{
  double
    value;

  int
    x,
    y;

  unsigned int
    flags,
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=ParseGeometry((char *) geometry,&x,&y,&width,&height);
  return((flags != NoValue) || sscanf(geometry,"%lf",&value));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyImages deallocates memory associated with a linked list
%  of images.
%
%  The format of the DestroyImages method is:
%
%      void DestroyImages(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void DestroyImages(Image *image)
{
  Image
    *next_image;

  /*
    Proceed to the top of the image list.
  */
  while (image->previous != (Image *) NULL)
    image=image->previous;
  do
  {
    /*
      Destroy this image.
    */
    next_image=image->next;
    if (next_image != (Image *)NULL)
      next_image->previous=(Image *)NULL;
    DestroyImage(image);
    image=next_image;
  } while (image != (Image *) NULL);
}

Export void DestroyImage(Image *image)
{
  /*
    Close image.
  */
  if (image->file != (FILE *) NULL)
    {
      CloseBlob(image);
      if (image->temporary)
        (void) remove(image->filename);
    }
  /*
    Deallocate the image comments.
  */
  if (image->comments != (char *) NULL)
    FreeMemory((char *) image->comments);
  /*
    Deallocate the image label.
  */
  if (image->label != (char *) NULL)
    FreeMemory((char *) image->label);
  /*
    Deallocate the image montage directory.
  */
  if (image->montage != (char *) NULL)
    FreeMemory((char *) image->montage);
  if (image->directory != (char *) NULL)
    FreeMemory((char *) image->directory);
  /*
    Deallocate the image colormap.
  */
  if (image->colormap != (ColorPacket *) NULL)
    FreeMemory((char *) image->colormap);
  /*
    Deallocate the image ICC profile.
  */
  if (image->color_profile.length > 0)
    FreeMemory((char *) image->color_profile.info);
  /*
    Deallocate the image IPTC profile.
  */
  if (image->iptc_profile.length > 0)
    FreeMemory((char *) image->iptc_profile.info);
  /*
    Deallocate the image signature.
  */
  if (image->signature != (char *) NULL)
    FreeMemory((char *) image->signature);
  /*
    Deallocate the image pixels.
  */
  if (image->pixels != (RunlengthPacket *) NULL)
    FreeMemory((char *) image->pixels);
  if (image->packed_pixels != (unsigned char *) NULL)
    FreeMemory((char *) image->packed_pixels);
  /*
    Deallocate the image page geometry.
  */
  if (image->page != (char *) NULL)
    FreeMemory((char *) image->page);
  if (!image->orphan)
    {
      /*
        Unlink from linked list.
      */
      if (image->previous != (Image *) NULL)
        {
          if (image->next != (Image *) NULL)
            image->previous->next=image->next;
          else
            image->previous->next=(Image *) NULL;
        }
      if (image->next != (Image *) NULL)
        {
          if (image->previous != (Image *) NULL)
            image->next->previous=image->previous;
          else
            image->next->previous=(Image *) NULL;
        }
    }
  /*
    Deallocate the image structure.
  */
  FreeMemory((char *) image);
  image=(Image *) NULL;
}

Export Image *CloneImage(const Image *image,const unsigned int columns,
  const unsigned int rows,const unsigned int clone_pixels)
{
  Image
    *clone_image;

  register int
    i;

  /*
    Allocate image structure.
  */
  clone_image=(Image *) AllocateMemory(sizeof(Image));
  if (clone_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Allocate the image pixels.
  */
  *clone_image=(*image);
  clone_image->columns=columns;
  clone_image->rows=rows;
  if (!clone_pixels)
    clone_image->packets=clone_image->columns*clone_image->rows;
  clone_image->pixels=(RunlengthPacket *) AllocateMemory((unsigned int)
    clone_image->packets*sizeof(RunlengthPacket));
  if (clone_image->pixels == (RunlengthPacket *) NULL)
    return((Image *) NULL);
  if (!clone_pixels)
    {
      clone_image->tainted=True;
      SetImage(clone_image);
    }
  else
    {
      register RunlengthPacket
        *p,
        *q;

      /*
        Copy image pixels.
      */
      p=image->pixels;
      q=clone_image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        *q=(*p);
        p++;
        q++;
      }
    }
  clone_image->packed_pixels=(unsigned char *) NULL;
  clone_image->comments=(char *) NULL;
  if (image->comments != (char *) NULL)
    (void) CloneString(&clone_image->comments,image->comments);
  clone_image->label=(char *) NULL;
  if (image->label != (char *) NULL)
    (void) CloneString(&clone_image->label,image->label);
  clone_image->montage=(char *) NULL;
  if (clone_pixels)
    if (image->montage != (char *) NULL)
      (void) CloneString(&clone_image->montage,image->montage);
  clone_image->directory=(char *) NULL;
  if (clone_pixels)
    if (image->directory != (char *) NULL)
      (void) CloneString(&clone_image->directory,image->directory);
  clone_image->signature=(char *) NULL;
  if (clone_pixels)
    if (image->signature != (char *) NULL)
      (void) CloneString(&clone_image->signature,image->signature);
  clone_image->page=(char *) NULL;
  if (clone_pixels)
    if (image->page != (char *) NULL)
      (void) CloneString(&clone_image->page,image->page);
  if (image->colormap != (ColorPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      clone_image->colormap=(ColorPacket *)
        AllocateMemory(image->colors*sizeof(ColorPacket));
      if (clone_image->colormap == (ColorPacket *) NULL)
        return((Image *) NULL);
      for (i=0; i < (int) image->colors; i++)
        clone_image->colormap[i]=image->colormap[i];
    }
  if (image->color_profile.length > 0)
    {
      /*
        Allocate and copy the image ICC profile.
      */
      clone_image->color_profile.info=(unsigned char *)
        AllocateMemory(image->color_profile.length*sizeof(unsigned char));
      if (clone_image->color_profile.info == (unsigned char *) NULL)
        return((Image *) NULL);
      for (i=0; i < (int) image->color_profile.length; i++)
        clone_image->color_profile.info[i]=image->color_profile.info[i];
    }
  if (image->iptc_profile.length > 0)
    {
      /*
        Allocate and copy the image IPTC profile.
      */
      clone_image->iptc_profile.info=(unsigned char *)
        AllocateMemory(image->iptc_profile.length*sizeof(unsigned char));
      if (clone_image->iptc_profile.info == (unsigned char *) NULL)
        return((Image *) NULL);
      for (i=0; i < (int) image->iptc_profile.length; i++)
        clone_image->iptc_profile.info[i]=image->iptc_profile.info[i];
    }
  if (image->orphan)
    {
      clone_image->file=(FILE *) NULL;
      clone_image->previous=(Image *) NULL;
      clone_image->next=(Image *) NULL;
    }
  else
    {
      /*
        Link image into image list.
      */
      if (clone_image->previous != (Image *) NULL)
        clone_image->previous->next=clone_image;
      if (clone_image->next != (Image *) NULL)
        clone_image->next->previous=clone_image;
    }
  clone_image->orphan=False;
  return(clone_image);
}

Export void SyncImage(Image *image)
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  if (image->class == DirectClass)
    return;
  for (i=0; i < (int) image->colors; i++)
  {
    image->colormap[i].index=0;
    image->colormap[i].flags=0;
  }
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    index=p->index;
    p->red=image->colormap[index].red;
    p->green=image->colormap[index].green;
    p->blue=image->colormap[index].blue;
    p++;
  }
}

Export void CondenseImage(Image *image)
{
  register int
    i,
    runlength;

  register long
    packets;

  register RunlengthPacket
    *p,
    *q;

  if ((image->columns == 0) || (image->rows == 0))
    return;
  if (image->pixels == (RunlengthPacket *) NULL)
    return;
  /*
    Compress image pixels.
  */
  p=image->pixels;
  runlength=p->length+1;
  packets=0;
  q=image->pixels;
  q->length=MaxRunlength;
  if (image->matte || (image->colorspace == CMYKColorspace))
    for (i=0; i < (int) (image->columns*image->rows); i++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && (p->index == q->index) &&
          ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          *q=(*p);
          q->length=0;
        }
    }
  else
    for (i=0; i < (int) (image->columns*image->rows); i++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          *q=(*p);
          q->length=0;
        }
    }
  SetRunlengthPackets(image,packets);
}

Export void AllocateNextImage(const ImageInfo *image_info,Image *image)
{
  /*
    Allocate image structure.
  */
  image->next=AllocateImage(image_info);
  if (image->next == (Image *) NULL)
    return;
  (void) strcpy(image->next->filename,image->filename);
  if (image_info != (ImageInfo *) NULL)
    (void) strcpy(image->next->filename,image_info->filename);
  image->next->file=image->file;
  image->next->filesize=image->filesize;
  image->next->scene=image->scene+1;
  image->next->previous=image;
}

Export void TransformRGBImage(Image *image,const ColorspaceType colorspace)
{
#define B (MaxRGB+1)*2
#define G (MaxRGB+1)
#define R 0
#define TransformRGBImageText  "  Transforming image colors...  "

  static const Quantum
    sRGBMap[351] =
    {
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,
       14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
       28,  29,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
       41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,
       55,  56,  57,  58,  59,  60,  61,  62,  63,  65,  66,  67,  68,  69,
       70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,
       84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  95,  96,  97,  98,
       99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
      114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
      128, 129, 130, 131, 132, 133, 135, 136, 137, 138, 139, 140, 141, 142,
      143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156,
      157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
      171, 172, 173, 174, 175, 175, 176, 177, 178, 179, 180, 181, 182, 183,
      184, 185, 186, 187, 187, 188, 189, 190, 191, 192, 193, 194, 194, 195,
      196, 197, 198, 199, 199, 200, 201, 202, 203, 203, 204, 205, 206, 207,
      207, 208, 209, 210, 210, 211, 212, 213, 213, 214, 215, 215, 216, 217,
      218, 218, 219, 220, 220, 221, 222, 222, 223, 223, 224, 225, 225, 226,
      227, 227, 228, 228, 229, 229, 230, 230, 231, 232, 232, 233, 233, 234,
      234, 235, 235, 235, 236, 236, 237, 237, 238, 238, 238, 239, 239, 240,
      240, 240, 241, 241, 242, 242, 242, 243, 243, 243, 243, 244, 244, 244,
      245, 245, 245, 245, 246, 246, 246, 247, 247, 247, 247, 247, 248, 248,
      248, 248, 249, 249, 249, 249, 249, 249, 250, 250, 250, 250, 250, 250,
      251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255,
      255
    },
    YCCMap[351] =  /* Photo CD information beyond 100% white, Gamma 2.2 */
    {
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,
        14, 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
        28, 29,  30,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,
        43, 45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  56,  57,  58,
        59, 60,  61,  62,  63,  64,  66,  67,  68,  69,  70,  71,  72,  73,
        74, 76,  77,  78,  79,  80,  81,  82,  83,  84,  86,  87,  88,  89,
        90, 91,  92,  93,  94,  95,  97,  98,  99, 100, 101, 102, 103, 104,
      105, 106, 107, 108, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
      120, 121, 122, 123, 124, 125, 126, 127, 129, 130, 131, 132, 133, 134,
      135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148,
      149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162,
      163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
      176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189,
      190, 191, 192, 193, 193, 194, 195, 196, 197, 198, 199, 200, 201, 201,
      202, 203, 204, 205, 206, 207, 207, 208, 209, 210, 211, 211, 212, 213,
      214, 215, 215, 216, 217, 218, 218, 219, 220, 221, 221, 222, 223, 224,
      224, 225, 226, 226, 227, 228, 228, 229, 230, 230, 231, 232, 232, 233,
      234, 234, 235, 236, 236, 237, 237, 238, 238, 239, 240, 240, 241, 241,
      242, 242, 243, 243, 244, 244, 245, 245, 245, 246, 246, 247, 247, 247,
      248, 248, 248, 249, 249, 249, 249, 250, 250, 250, 250, 251, 251, 251,
      251, 251, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253, 253,
      253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255
    };
  double
    *blue,
    *green,
    *red;

  Quantum
    *range_table;

  register int
    i,
    x,
    y,
    z;

  register Quantum
    *range_limit;

  register RunlengthPacket
    *p;

  if ((image->colorspace == CMYKColorspace) && (colorspace == RGBColorspace))
    {
      unsigned int
        black,
        cyan,
        magenta,
        yellow;

      /*
        Transform image from CMYK to RGB.
      */
      image->colorspace=RGBColorspace;
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        cyan=p->red;
        magenta=p->green;
        yellow=p->blue;
        black=p->index;
        if ((cyan+black) > MaxRGB)
          p->red=0;
        else
          p->red=MaxRGB-(cyan+black);
        if ((magenta+black) > MaxRGB)
          p->green=0;
        else
          p->green=MaxRGB-(magenta+black);
        if ((yellow+black) > MaxRGB)
          p->blue=0;
        else
          p->blue=MaxRGB-(yellow+black);
        p->index=0;
        p++;
      }
      return;
    }
  if ((colorspace == RGBColorspace) || (colorspace == GRAYColorspace) ||
      (colorspace == TransparentColorspace))
    return;
  /*
    Allocate the tables.
  */
  red=(double *) AllocateMemory(3*(MaxRGB+1)*sizeof(double));
  green=(double *) AllocateMemory(3*(MaxRGB+1)*sizeof(double));
  blue=(double *) AllocateMemory(3*(MaxRGB+1)*sizeof(double));
  range_table=(Quantum *) AllocateMemory(4*(MaxRGB+1)*sizeof(Quantum));
  if ((red == (double *) NULL) || (green == (double *) NULL) ||
      (blue == (double *) NULL) || (range_table == (Quantum *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to transform color space",
        "Memory allocation failed");
      return;
    }
  /*
    Initialize tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(Quantum) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  for (i=0; i <= MaxRGB; i++)
    range_table[i+(MaxRGB+1)*3]=MaxRGB;
  range_limit=range_table+(MaxRGB+1);
  switch (colorspace)
  {
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          R = I1+1.00000*I2-0.66668*I3
          G = I1+0.00000*I2+1.33333*I3
          B = I1-1.00000*I2-0.66668*I3

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=i;
        green[i+R]=0.5*(2.0*i-MaxRGB);
        blue[i+R]=(-0.33334)*(2.0*i-MaxRGB);
        red[i+G]=i;
        green[i+G]=0.0;
        blue[i+G]=0.666665*(2.0*i-MaxRGB);
        red[i+B]=i;
        green[i+B]=(-0.5)*(2.0*i-MaxRGB);
        blue[i+B]=(-0.33334)*(2.0*i-MaxRGB);
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          R = Y            +1.032096*C2
          G = Y-0.326904*C1-0.704445*C2
          B = Y+1.685070*C1

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=1.40200*i;
        green[i+R]=0.0;
        blue[i+R]=1.88000*(i-(double) UpScale(137));
        red[i+G]=1.40200*i;
       green[i+G]=(-0.444066)*(i-(double) UpScale(156));
        blue[i+G]=(-0.95692)*(i-(double) UpScale(137));
        red[i+B]=1.40200*i;
        green[i+B]=2.28900*(i-(double) UpScale(156));
        blue[i+B]=0.0;
        range_table[i+(MaxRGB+1)]=(Quantum) UpScale(sRGBMap[DownScale(i)]);
      }
      for ( ; i < (int) UpScale(351); i++)
        range_table[i+(MaxRGB+1)]=(Quantum) UpScale(sRGBMap[DownScale(i)]);
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables:

          R =  3.240479*R-1.537150*G-0.498535*B
          G = -0.969256*R+1.875992*G+0.041556*B
          B =  0.055648*R-0.204043*G+1.057311*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=3.240479*i;
        green[i+R]=(-1.537150)*i;
        blue[i+R]=(-0.498535)*i;
        red[i+G]=(-0.969256)*i;
        green[i+G]=1.875992*i;
        blue[i+G]=0.041556*i;
        red[i+B]=0.055648*i;
        green[i+B]=(-0.204043)*i;
        blue[i+B]=1.057311*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          R = Y            +1.402000*Cr
          G = Y-0.344136*Cb-0.714136*Cr
          B = Y+1.772000*Cb

        Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=i;
        green[i+R]=0.0;
        blue[i+R]=(1.402000*0.5)*(2.0*i-MaxRGB);
        red[i+G]=i;
        green[i+G]=(-0.344136*0.5)*(2.0*i-MaxRGB);
        blue[i+G]=(-0.714136*0.5)*(2.0*i-MaxRGB);
        red[i+B]=i;
        green[i+B]=(1.772000*0.5)*(2.0*i-MaxRGB);
        blue[i+B]=0.0;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          R = Y            +1.340762*C2
          G = Y-0.317038*C1-0.682243*C2
          B = Y+1.632639*C1

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=1.3584*i;
        green[i+R]=0.0;
        blue[i+R]=1.8215*(i-(double) UpScale(137));
        red[i+G]=1.3584*i;
        green[i+G]=(-0.4302726)*(i-(double) UpScale(156));
        blue[i+G]=(-0.9271435)*(i-(double) UpScale(137));
        red[i+B]=1.3584*i;
        green[i+B]=2.2179*(i-(double) UpScale(156));
        blue[i+B]=0.0;
        range_table[i+(MaxRGB+1)]=(Quantum) UpScale(YCCMap[DownScale(i)]);
      }
      for ( ; i < (int) UpScale(351); i++)
        range_table[i+(MaxRGB+1)]=(Quantum) UpScale(YCCMap[DownScale(i)]);
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          R = Y+0.95620*I+0.62140*Q
          G = Y-0.27270*I-0.64680*Q
          B = Y-1.10370*I+1.70060*Q

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=i;
        green[i+R]=0.4781*(2.0*i-MaxRGB);
        blue[i+R]=0.3107*(2.0*i-MaxRGB);
        red[i+G]=i;
        green[i+G]=(-0.13635)*(2.0*i-MaxRGB);
        blue[i+G]=(-0.3234)*(2.0*i-MaxRGB);
        red[i+B]=i;
        green[i+B]=(-0.55185)*(2.0*i-MaxRGB);
        blue[i+B]=0.8503*(2.0*i-MaxRGB);
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          R = Y            +1.402000*C2
          G = Y-0.344136*C1+0.714136*C2
          B = Y+1.772000*C1

        Pb and Pr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=i;
        green[i+R]=0.0;
        blue[i+R]=0.701*(2.0*i-MaxRGB);
        red[i+G]=i;
        green[i+G]=(-0.172068)*(2.0*i-MaxRGB);
        blue[i+G]=0.357068*(2.0*i-MaxRGB);
        red[i+B]=i;
        green[i+B]=0.886*(2.0*i-MaxRGB);
        blue[i+B]=0.0;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          R = Y          +1.13980*V
          G = Y-0.39380*U-0.58050*V
          B = Y+2.02790*U

        U and V, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=i;
        green[i+R]=0.0;
        blue[i+R]=0.5699*(2.0*i-MaxRGB);
        red[i+G]=i;
        green[i+G]=(-0.1969)*(2.0*i-MaxRGB);
        blue[i+G]=(-0.29025)*(2.0*i-MaxRGB);
        red[i+B]=i;
        green[i+B]=1.01395*(2.0*i-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
  }
  /*
    Convert to RGB.
  */
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Convert DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        x=p->red;
        y=p->green;
        z=p->blue;
        p->red=range_limit[(int) (red[x+R]+green[y+R]+blue[z+R])];
        p->green=range_limit[(int) (red[x+G]+green[y+G]+blue[z+G])];
        p->blue=range_limit[(int) (red[x+B]+green[y+B]+blue[z+B])];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        x=image->colormap[i].red;
        y=image->colormap[i].green;
        z=image->colormap[i].blue;
        image->colormap[i].red=
          range_limit[(int) (red[x+R]+green[y+R]+blue[z+R])];
        image->colormap[i].green=
          range_limit[(int) (red[x+G]+green[y+G]+blue[z+G])];
        image->colormap[i].blue=
          range_limit[(int) (red[x+B]+green[y+B]+blue[z+B])];
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocated memory.
  */
  FreeMemory((char *) range_table);
  FreeMemory((char *) blue);
  FreeMemory((char *) green);
  FreeMemory((char *) red);
}

Export void RGBTransformImage(Image *image,const ColorspaceType colorspace)
{
#define RGBTransformImageText  "  Transforming image colors...  "
#define X 0
#define Y (MaxRGB+1)
#define Z (MaxRGB+1)*2

  double
    tx,
    ty,
    tz;

  Quantum
    *range_table;

  register double
    *x,
    *y,
    *z;

  register int
    blue,
    green,
    i,
    red;

  register Quantum
    *range_limit;

  register RunlengthPacket
    *p;

  assert(image != (Image *) NULL);
  if ((colorspace == RGBColorspace) || (colorspace == TransparentColorspace))
    return;
  if (colorspace == CMYKColorspace)
    {
      Quantum
        black,
        cyan,
        magenta,
        yellow;

      /*
        Convert RGB to CMYK colorspace.
      */
      image->colorspace=CMYKColorspace;
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        cyan=MaxRGB-p->red;
        magenta=MaxRGB-p->green;
        yellow=MaxRGB-p->blue;
        black=cyan;
        if (magenta < black)
          black=magenta;
        if (yellow < black)
          black=yellow;
        p->red=cyan;
        p->green=magenta;
        p->blue=yellow;
        p->index=black;
        p++;
      }
      return;

    }
  if (colorspace == GRAYColorspace)
    {
      /*
        Return if the image is already gray_scale.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        if ((p->red != p->green) || (p->green != p->blue))
          break;
        p++;
      }
      if (i == (int) image->packets)
        return;
    }
  /*
    Allocate the tables.
  */
  x=(double *) AllocateMemory(3*(MaxRGB+1)*sizeof(double));
  y=(double *) AllocateMemory(3*(MaxRGB+1)*sizeof(double));
  z=(double *) AllocateMemory(3*(MaxRGB+1)*sizeof(double));
  range_table=(Quantum *) AllocateMemory(4*(MaxRGB+1)*sizeof(Quantum));
  if ((x == (double *) NULL) || (y == (double *) NULL) ||
      (z == (double *) NULL) || (range_table == (Quantum *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Unable to transform color space",
        "Memory allocation failed");
      return;
    }
  /*
    Pre-compute conversion tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(Quantum) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  for (i=0; i <= MaxRGB; i++)
    range_table[i+(MaxRGB+1)*3]=MaxRGB;
  range_limit=range_table+(MaxRGB+1);
  tx=0;
  ty=0;
  tz=0;
  switch (colorspace)
  {
    case GRAYColorspace:
    {
      /*
        Initialize GRAY tables:

          G = 0.29900*R+0.58700*G+0.11400*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=0.299*i;
        y[i+X]=0.587*i;
        z[i+X]=0.114*i;
        x[i+Y]=0.299*i;
        y[i+Y]=0.587*i;
        z[i+Y]=0.114*i;
        x[i+Z]=0.299*i;
        y[i+Z]=0.587*i;
        z[i+Z]=0.114*i;
      }
      break;
    }
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          I1 = 0.33333*R+0.33334*G+0.33333*B
          I2 = 0.50000*R+0.00000*G-0.50000*B
          I3 =-0.25000*R+0.50000*G-0.25000*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=(MaxRGB+1) >> 1;
      tz=(MaxRGB+1) >> 1;
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=0.33333*i;
        y[i+X]=0.33334*i;
        z[i+X]=0.33333*i;
        x[i+Y]=0.5*i;
        y[i+Y]=0.0;
        z[i+Y]=(-0.5)*i;
        x[i+Z]=(-0.25)*i;
        y[i+Z]=0.5*i;
        z[i+Z]=(-0.25)*i;
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      ty=UpScale(156);
      tz=UpScale(137);
      for (i=0; i <= (int) (0.018*MaxRGB); i++)
      {
        x[i+X]=0.003962014134275617*MaxRGB*i;
        y[i+X]=0.007778268551236748*MaxRGB*i;
        z[i+X]=0.001510600706713781*MaxRGB*i;
        x[i+Y]=(-0.002426619775463276)*MaxRGB*i;
        y[i+Y]=(-0.004763965913702149)*MaxRGB*i;
        z[i+Y]=0.007190585689165425*MaxRGB*i;
        x[i+Z]=0.006927257754597858*MaxRGB*i;
        y[i+Z]=(-0.005800713697502058)*MaxRGB*i;
        z[i+Z]=(-0.0011265440570958)*MaxRGB*i;
      }
      for ( ; i <= MaxRGB; i++)
      {
        x[i+X]=0.2201118963486454*(1.099*i-0.099);
        y[i+X]=0.4321260306242638*(1.099*i-0.099);
        z[i+X]=0.08392226148409894*(1.099*i-0.099);
        x[i+Y]=(-0.1348122097479598)*(1.099*i-0.099);
        y[i+Y]=(-0.2646647729834528)*(1.099*i-0.099);
        z[i+Y]=0.3994769827314126*(1.099*i-0.099);
        x[i+Z]=0.3848476530332144*(1.099*i-0.099);
        y[i+Z]=(-0.3222618720834477)*(1.099*i-0.099);
        z[i+Z]=(-0.06258578094976668)*(1.099*i-0.099);
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables:

          X = 0.412453*X+0.357580*Y+0.180423*Z
          Y = 0.212671*X+0.715160*Y+0.072169*Z
          Z = 0.019334*X+0.119193*Y+0.950227*Z
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=0.412453*i;
        y[i+X]=0.35758*i;
        z[i+X]=0.180423*i;
        x[i+Y]=0.212671*i;
        y[i+Y]=0.71516*i;
        z[i+Y]=0.072169*i;
        x[i+Z]=0.019334*i;
        y[i+Z]=0.119193*i;
        z[i+Z]=0.950227*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          Y =  0.299000*R+0.587000*G+0.114000*B
          Cb= -0.168736*R-0.331264*G+0.500000*B
          Cr=  0.500000*R-0.418688*G-0.083168*B

        Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=(MaxRGB+1) >> 1;
      tz=(MaxRGB+1) >> 1;
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=0.299*i;
        y[i+X]=0.587*i;
        z[i+X]=0.114*i;
        x[i+Y]=(-0.16873)*i;
        y[i+Y]=(-0.331264)*i;
        z[i+Y]=0.500000*i;
        x[i+Z]=0.500000*i;
        y[i+Z]=(-0.418688)*i;
        z[i+Z]=(-0.081312)*i;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B
        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      ty=UpScale(156);
      tz=UpScale(137);
      for (i=0; i <= (int) (0.018*MaxRGB); i++)
      {
        x[i+X]=0.003962014134275617*MaxRGB*i;
        y[i+X]=0.007778268551236748*MaxRGB*i;
        z[i+X]=0.001510600706713781*MaxRGB*i;
        x[i+Y]=(-0.002426619775463276)*MaxRGB*i;
        y[i+Y]=(-0.004763965913702149)*MaxRGB*i;
        z[i+Y]=0.007190585689165425*MaxRGB*i;
        x[i+Z]=0.006927257754597858*MaxRGB*i;
        y[i+Z]=(-0.005800713697502058)*MaxRGB*i;
        z[i+Z]=(-0.0011265440570958)*MaxRGB*i;
      }
      for ( ; i <= MaxRGB; i++)
      {
        x[i+X]=0.2201118963486454*(1.099*i-0.099);
        y[i+X]=0.4321260306242638*(1.099*i-0.099);
        z[i+X]=0.08392226148409894*(1.099*i-0.099);
        x[i+Y]=(-0.1348122097479598)*(1.099*i-0.099);
        y[i+Y]=(-0.2646647729834528)*(1.099*i-0.099);
        z[i+Y]=0.3994769827314126*(1.099*i-0.099);
        x[i+Z]=0.3848476530332144*(1.099*i-0.099);
        y[i+Z]=(-0.3222618720834477)*(1.099*i-0.099);
        z[i+Z]=(-0.06258578094976668)*(1.099*i-0.099);
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          Y = 0.29900*R+0.58700*G+0.11400*B
          I = 0.59600*R-0.27400*G-0.32200*B
          Q = 0.21100*R-0.52300*G+0.31200*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=(MaxRGB+1) >> 1;
      tz=(MaxRGB+1) >> 1;
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=0.299*i;
        y[i+X]=0.587*i;
        z[i+X]=0.114*i;
        x[i+Y]=0.596*i;
        y[i+Y]=(-0.274)*i;
        z[i+Y]=(-0.322)*i;
        x[i+Z]=0.211*i;
        y[i+Z]=(-0.523)*i;
        z[i+Z]=0.312*i;
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          Y =  0.299000*R+0.587000*G+0.114000*B
          Pb= -0.168736*R-0.331264*G+0.500000*B
          Pr=  0.500000*R-0.418688*G-0.081312*B
        Pb and Pr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=(MaxRGB+1) >> 1;
      tz=(MaxRGB+1) >> 1;
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=0.299*i;
        y[i+X]=0.587*i;
        z[i+X]=0.114*i;
        x[i+Y]=(-0.168736)*i;
        y[i+Y]=(-0.331264)*i;
        z[i+Y]=0.5*i;
        x[i+Z]=0.5*i;
        y[i+Z]=(-0.418688)*i;
        z[i+Z]=(-0.081312)*i;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          U = -0.14740*R-0.28950*G+0.43690*B
          V =  0.61500*R-0.51500*G-0.10000*B

        U and V, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
      */
      ty=(MaxRGB+1) >> 1;
      tz=(MaxRGB+1) >> 1;
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=0.299*i;
        y[i+X]=0.587*i;
        z[i+X]=0.114*i;
        x[i+Y]=(-0.1474)*i;
        y[i+Y]=(-0.2895)*i;
        z[i+Y]=0.4369*i;
        x[i+Z]=0.615*i;
        y[i+Z]=(-0.515)*i;
        z[i+Z]=(-0.1)*i;
      }
      break;
    }
  }
  /*
    Convert from RGB.
  */
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      /*
        Convert DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        red=p->red;
        green=p->green;
        blue=p->blue;
        p->red=range_limit[(int) (x[red+X]+y[green+X]+z[blue+X]+tx)];
        p->green=range_limit[(int) (x[red+Y]+y[green+Y]+z[blue+Y]+ty)];
        p->blue=range_limit[(int) (x[red+Z]+y[green+Z]+z[blue+Z]+tz)];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        red=image->colormap[i].red;
        green=image->colormap[i].green;
        blue=image->colormap[i].blue;
        image->colormap[i].red=
          range_limit[(int) (x[red+X]+y[green+X]+z[blue+X]+tx)];
        image->colormap[i].green=
          range_limit[(int) (x[red+Y]+y[green+Y]+z[blue+Y]+ty)];
        image->colormap[i].blue=
          range_limit[(int) (x[red+Z]+y[green+Z]+z[blue+Z]+tz)];
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocated memory.
  */
  FreeMemory((char *) range_table);
  FreeMemory((char *) z);
  FreeMemory((char *) y);
  FreeMemory((char *) x);
}

