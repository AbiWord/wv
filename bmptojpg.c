#include <magick.h>
#include <string.h>
int bmptojpg(char *prefix);

#if 0
int main (int argc, char **argv)
	{
	return(bmptojpg(argv[1]));
	}
#endif

int bmptojpg(char *prefix)
	{
	Image *image;
	ImageInfo image_info;
	char buffer[4096];

	GetImageInfo (&image_info);
	sprintf(buffer, "%s.bmp",prefix);
	strcpy (image_info.filename, buffer);
	image = ReadBMPImage (&image_info);
	if (image == (Image *) NULL)
		return(1);
	sprintf(buffer, "%s.jpg",prefix);
	strcpy (image->filename, buffer);
	WriteJPEGImage (&image_info, image);
	DestroyImage (image);
	return(0);
	}
