/* About */
/* *
 * Image loading implementation based on GraphicsMagick C API
 * WARNING: this implementation consumes tons of memory for no reason,
 * beware or tell me how to fix it.
 * */

/* Macros */
#include "image.h"
#include "imagelist.h"
#include "config.h"
#include "stringutils.h"
#include <magick/api.h>

/* Local scope */
static unsigned long S_I_get_mean ( PixelPacket * pixels );
static imgdata * S_I_hash_from_thumb ( Image * image, string * str );

static imgdata *
S_I_hash_from_thumb ( Image * image, string * str )
{
	unsigned short i = 0;
	PixelPacket * pixels;
	unsigned long mean;

	imgdata * imdat = malloc(sizeof(imgdata));
	imdat->path = str;
	imdat->hash = 0;
	imdat->group = NULL;

	pixels = GetImagePixels( image, 0, 0, config.avghash_side, config.avghash_side );
	mean = S_I_get_mean(pixels);

	for ( ; i < config.square; ++i )
		imdat->hash |= (HASHTYPE)( (unsigned)( mean - pixels[i].red ) >> ( sizeof(int) * 8 - 1 ) ) << i;

	return imdat;
}

static unsigned long
S_I_get_mean ( PixelPacket * pixels )
{
	unsigned long colr = 0;

	unsigned long i = 0;
	for ( ; i < config.square; ++i )
		colr += pixels[i].red;

	return colr / config.square;
}

/* Global scope */
void
I_init ( char * path )
{
	InitializeMagick(path);
}

void
I_finish ( void )
{
	DestroyMagick();
}

imgdata *
I_process ( string * instr )
{
	ExceptionInfo ex;
	ImageInfo * image_info = CloneImageInfo((ImageInfo *)NULL);
	Image * resized_image;
	string * str = stringcopy(instr);
	imgdata * retvalue = NULL;

	GetExceptionInfo(&ex);

	snprintf( image_info->filename, str->length + 1, "%s", str->s );

	{
		Image * image = ReadImage( image_info, &ex );

		if ( !image )
		{
			free_string(str);
			goto S_I_check_onexit;
		}

		resized_image = ResizeImage( image, config.avghash_side, config.avghash_side, LanczosFilter, 1.0 ,&ex );
		if ( resized_image == (Image *)NULL )
		{
			fputs( "Failed to resize.\n", stderr );
			CatchException(&ex);

			free_string(str);
			goto S_I_check_onexit;
		}

		GrayscalePseudoClassImage( resized_image, 1 );
		retvalue = S_I_hash_from_thumb( resized_image, str );
		DestroyImage(resized_image);
		DestroyImage(image);
	}

	S_I_check_onexit:
	DestroyImageInfo(image_info);
	DestroyExceptionInfo(&ex);

	return retvalue;
}
