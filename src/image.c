#include "image.h"
#include "config.h"
#include "stringutils.h"
#include <magick/api.h>
#include <string.h>

static void S_I_check ( char * path );
static PixelPacket S_I_get_mean ( HistogramColorPacket *, unsigned long );

static PixelPacket S_I_get_mean ( HistogramColorPacket * hist, unsigned long colnumb )
{
//	fprintf( stderr, "Calculating mean color from %lu values...\n", colnumb );
//	fprintf( stderr, "Max value: %d\n", MaxRGB );

	PixelPacket pp;
	unsigned long colr = 0;
	unsigned long colg = 0;
	unsigned long colb = 0;
	unsigned long cola = 0;

	for ( unsigned long i = 0; i < colnumb; ++i )
	{
		colr += hist[i].pixel.red;
		colg += hist[i].pixel.green;
		colb += hist[i].pixel.blue;
		cola += hist[i].pixel.opacity;
	}

	pp.red = colr / colnumb;
	pp.green = colg / colnumb;
	pp.blue = colb / colnumb;
	pp.opacity = cola / colnumb;

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

//	QuantizeInfo quant;
//	GetQuantizeInfo(&quant);
//	quant.number_colors = 64;
//	QuantizeImage( &quant, resize_image );

	DestroyImage(image);

	if ( resize_image == (Image *)NULL )
	{
		fputs( "Failed to resize.\n", stderr );
		CatchException(&ex);
		goto S_I_check_onexit;
	}

	GrayscalePseudoClassImage( resize_image, 1 );

	unsigned long colnumb = 0;
	HistogramColorPacket * hist = GetColorHistogram( resize_image, &colnumb, &ex );

	stringcat( str, "%s", ".thumb.jpg", image_info->magick );
	(void)strcpy( image_info->filename, str->s );
	FILE * output = fopen( str->s, "w" );

	fprintf( stderr, "Writing %s with res %lux%lu, colors: %lu...\n", image_info->filename, resize_image->columns, resize_image->rows, colnumb );

	if ( !WriteImagesFile( image_info, resize_image, output, &ex ) )
		CatchException(&resize_image->exception);

	fclose(output);
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
