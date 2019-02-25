/* Macros */
#include "image.h"
#include "imagelist.h"
#include "config.h"
#include "stringutils.h"
#include <magick/api.h>
#include <string.h>

/* Local scope */
static imagelist List;

static unsigned long S_I_get_mean ( PixelPacket * pixels );
static void S_I_compare ( img * img1, img * img2 );
static void S_I_hash_from_thumb ( Image * image, string * str );

static void
S_I_hash_from_thumb ( Image * image, string * str )
{
	unsigned short i = 0;
	PixelPacket * pixels;
	unsigned long mean;

	img * imagest;
	imagest = malloc(sizeof(img));
	imagest->path = str;
	imagest->hash = 0;

	pixels = GetImagePixels( image, 0, 0, config.avghash_side, config.avghash_side );
	mean = S_I_get_mean(pixels);

	for ( ; i < config.square; ++i )
		imagest->hash |= (uint_fast64_t)( (unsigned)( mean - pixels[i].red ) >> ( sizeof(int) * 8 - 1 ) ) << i;

	IL_add_to_imagelist( &List, imagest );
}

static void
S_I_compare ( img * img1, img * img2 )
{
	unsigned similar_bits = config.square;
	uint_fast64_t similarity_hash = img1->hash ^ img2->hash;

	fprintf( stderr, "Comparing %" PRIxFAST64 " with %" PRIxFAST64 "... ", img1->hash, img2->hash );

	while ( similarity_hash != 0 )
	{
		similar_bits--;
		similarity_hash &= similarity_hash - 1;
	}

	if ( similar_bits / (float)config.square >= config.precision )
		fprintf( stderr, "%s", " HIT! " );

	fprintf( stderr, " similar: %u / %u\n", similar_bits, config.square );
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
I_stats ( void )
{
	img * iter = List.head;
	size_t i = 0;

	fprintf( stderr, "Images processed successfully: %lu\n", List.length );

	for ( ; i < List.length; ++i )
	{
		fprintf( stderr, "%s: %" PRIxFAST64 "\n", iter->path->s, iter->hash );
		iter = iter->next;
	}
}

void
I_finish ( void )
{
	IL_free_imagelist(&List);
}

void
I_compare_all ( void )
{
	img * currenthead = List.head;
	size_t i = 0;
	size_t j = i + 1;

	for ( ; i + 1 < List.length; ++i )
	{
		img * currenttail = currenthead->next;
		for ( j = i + 1; j < List.length; ++j )
		{
			S_I_compare( currenthead, currenttail );
			currenttail = currenttail->next;
		}

		currenthead = currenthead->next;
	}
}

void
I_process ( string * instr )
{
	ExceptionInfo ex;
	ImageInfo * image_info;
	Image * resized_image;

	string * str = stringcopy(instr);

	InitializeMagick(NULL);
	GetExceptionInfo(&ex);

	image_info = CloneImageInfo((ImageInfo *)NULL);
	snprintf( image_info->filename, str->length + 1, "%s", str->s );

	{
		Image * image = ReadImage( image_info, &ex );

		if ( ex.severity != UndefinedException )
			CatchException(&ex);

		if ( !image )
		{
			free_string(str);
			goto S_I_check_onexit;
		}

		resized_image = ResizeImage( image, config.avghash_side, config.avghash_side, LanczosFilter, 1.0 ,&ex );
		DestroyImage(image);
	}

	if ( resized_image == (Image *)NULL )
	{
		fputs( "Failed to resize.\n", stderr );
		CatchException(&ex);

		free_string(str);
		goto S_I_check_onexit;
	}

	GrayscalePseudoClassImage( resized_image, 1 );
	S_I_hash_from_thumb( resized_image, str );
	DestroyImage(resized_image);

	S_I_check_onexit:
	DestroyImageInfo(image_info);
	DestroyExceptionInfo(&ex);
	DestroyMagick();
}
