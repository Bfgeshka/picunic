/* Macros */
#include "image.h"
#include "config.h"
#include "stringutils.h"
#include <magick/api.h>
#include <string.h>

/* Local scope */
static imagelist List;

static void S_I_check ( char * path );
static PixelPacket S_I_get_mean ( PixelPacket * pixels );
static unsigned char S_I_compare_color ( PixelPacket mean, PixelPacket compared );
static void S_I_add_to_list ( img * image );

static void
S_I_add_to_list ( img * image )
{
	if ( List.length == 0 )
	{
		List.length++;
		List.head = image;
		List.tail = image;
		return;
	}

	List.tail->next = image;
	List.tail = image;
	List.length++;
}

static unsigned char
S_I_compare_color ( PixelPacket mean, PixelPacket compared )
{
	int difference = mean.red - compared.red + mean.green - compared.green + mean.blue - compared.blue + mean.opacity - compared.opacity;
	unsigned short retvalue = ( difference > 0 ) ? 0 : 1;
	return retvalue;
}

static PixelPacket
S_I_get_mean ( PixelPacket * pixels )
{
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

//	fprintf( stderr, "Reading %s ...", image_info->filename );
	Image * image = ReadImage( image_info, &ex );

//	fprintf( stderr, " %lu frames\n", GetImageListLength(image) );
	if ( ex.severity != UndefinedException )
		CatchException(&ex);

	if ( !image )
	{
		fputs( "Null image data.\n", stderr );

		free_string(str);
		goto S_I_check_onexit;
	}

	Image * resize_image = ResizeImage( image, config.avghash_side, config.avghash_side, LanczosFilter, 1.0 ,&ex );
	DestroyImage(image);

	if ( resize_image == (Image *)NULL )
	{
		fputs( "Failed to resize.\n", stderr );
		CatchException(&ex);

		free_string(str);
		goto S_I_check_onexit;
	}

	GrayscalePseudoClassImage( resize_image, 1 );

	PixelPacket * pixels = GetImagePixels( resize_image, 0, 0, config.avghash_side, config.avghash_side );
	PixelPacket mean = S_I_get_mean(pixels);

	uint_fast64_t hash = 0;

	unsigned short square = config.avghash_side * config.avghash_side;
	for ( unsigned short i = 0; i < square; ++i )
		hash |= ( (uint_fast64_t)S_I_compare_color( mean, pixels[i] ) << i );

//	fprintf( stderr, "Image hash: %" PRIxFAST64 "\n", hash );

	img * imagest = malloc(sizeof(img));
	imagest->path = str;
	imagest->hash = hash;
	S_I_add_to_list(imagest);

	DestroyImage(resize_image);

	S_I_check_onexit:
//	free_string(str);
	DestroyImageInfo(image_info);
	DestroyExceptionInfo(&ex);
	DestroyMagick();
}

/* Global scope */
void
I_stats ( void )
{
	fprintf( stderr, "Images processed successfully: %zu\n", List.length );

	img * iter = List.head;
	for ( size_t i = 0; i < List.length; ++i )
	{
		fprintf( stderr, "%s: %" PRIxFAST64 "\n", iter->path->s, iter->hash );
		iter = iter->next;
	}
}

void
I_finish ( void )
{
	for ( size_t i = 0; i < List.length; ++i )
	{
		img * next = List.head->next;
		free_string(List.head->path);
		free(List.head);
		List.head = next;
	}
}

void
I_process ( char * path )
{
	S_I_check(path);
}
