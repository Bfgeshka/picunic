#include "image.h"
#include "config.h"
#include "stringutils.h"
#include <magick/api.h>
#include <string.h>

static void S_I_check ( char * path );
static PixelPacket S_I_get_mean ( PixelPacket * pixels );
static unsigned short S_I_compare_color ( PixelPacket mean, PixelPacket compared );

static unsigned short S_I_compare_color ( PixelPacket mean, PixelPacket compared )
{
	int difference = mean.red - compared.red + mean.green - compared.green + mean.blue - compared.blue + mean.opacity - compared.opacity;
	unsigned short retvalue = ( difference > 0 ) ? 0 : 1;
//	printf( "Comparing with %d %d %d %d, result: %d\n", compared.red, compared.green, compared.blue, compared.opacity, retvalue );
	return retvalue;
}

static PixelPacket S_I_get_mean ( PixelPacket * pixels )
{
//	fprintf( stderr, "Calculating mean color from %lu values...\n", colnumb );
//	fprintf( stderr, "Max value: %d\n", MaxRGB );

	PixelPacket pp;
	unsigned long colr = 0;
	unsigned long colg = 0;
	unsigned long colb = 0;
	unsigned long cola = 0;

	unsigned short pixelnumb = config.avghash_side * config.avghash_side;

	for ( unsigned long i = 0; i < pixelnumb; ++i )
	{
		colr += pixels[i].red;
		colg += pixels[i].green;
		colb += pixels[i].blue;
		cola += pixels[i].opacity;
	}

	pp.red = colr / pixelnumb;
	pp.green = colg / pixelnumb;
	pp.blue = colb / pixelnumb;
	pp.opacity = cola / pixelnumb;

	return pp;
}

static void
S_I_check ( char * path )
{
	ExceptionInfo ex;
	string * str = construct_string(2048);
	stringset( str, "%s", path );

	InitializeMagick(NULL);
	GetExceptionInfo(&ex);

	ImageInfo * image_info = CloneImageInfo((ImageInfo *)NULL);

	(void)strcpy( image_info->filename, str->s );

	fprintf( stderr, "Reading %s ...", image_info->filename );
	Image * image = ReadImage( image_info, &ex );

	fprintf( stderr, " %lu frames\n", GetImageListLength(image) );
	if ( ex.severity != UndefinedException )
		CatchException(&ex);

	if ( !image )
	{
		fputs( "Null image data.\n", stderr );
		goto S_I_check_onexit;
	}

	Image * resize_image = ResizeImage( image, config.avghash_side, config.avghash_side, LanczosFilter, 1.0 ,&ex );
	DestroyImage(image);

	if ( resize_image == (Image *)NULL )
	{
		fputs( "Failed to resize.\n", stderr );
		CatchException(&ex);
		goto S_I_check_onexit;
	}

	GrayscalePseudoClassImage( resize_image, 1 );

	PixelPacket * pixels = GetImagePixels( resize_image, 0, 0, config.avghash_side, config.avghash_side );
	PixelPacket mean = S_I_get_mean(pixels);
	fprintf( stderr, "Mean value: (%d %d %d %d)\n", mean.red, mean.green, mean.blue, mean.opacity );

	unsigned long long hash = 0;

	for ( unsigned short u = 0, i = 0; u < config.avghash_side; ++u )
		for ( unsigned short v = 0; v < config.avghash_side; ++v, ++i )
			hash |= ( S_I_compare_color( mean, pixels[i] ) << i );
//			if (  != 0 )
//				hash |= ( 0x00000001 << i );

	unsigned long long hashcp = hash;
	while (hashcp)
	{
		printf("%llu", (hashcp & 1));
		hashcp >>= 1;
	}
	printf("\n");

	fprintf( stderr, "Image hash: %llx\n", hash );

	DestroyImage(resize_image);

	S_I_check_onexit:
	free_string(str);
	DestroyImageInfo(image_info);
	DestroyExceptionInfo(&ex);
	DestroyMagick();
}

void
I_process ( char * path )
{
	S_I_check(path);
}
