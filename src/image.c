/* Macros */
#include "image.h"
#include "config.h"
#include "stringutils.h"
#include <magick/api.h>
#include <string.h>

/* Local scope */
static imagelist List;
static unsigned short PixInSquare;

static void S_I_check ( string * path );
static PixelPacket S_I_get_mean ( PixelPacket * pixels );
static void S_I_add_to_list ( img * image );
static void S_I_compare ( img * img1, img * img2 );

static void
S_I_compare ( img * img1, img * img2 )
{
	fprintf( stderr, "Comparing %" PRIxFAST64 " with %" PRIxFAST64 "... ", img1->hash, img2->hash );

	unsigned short similar_bits = PixInSquare;

	uint_fast64_t similarity_hash = img1->hash ^ img2->hash;
	while ( similarity_hash != 0 )
	{
		similar_bits--;
		similarity_hash &= similarity_hash - 1;
	}

	fprintf( stderr, " similar: %hu / %hu\n", similar_bits, PixInSquare );
}

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
S_I_check ( string * instr )
{
	ExceptionInfo ex;
	string * str = stringcopy(instr);

	InitializeMagick(NULL);
	GetExceptionInfo(&ex);

	ImageInfo * image_info = CloneImageInfo((ImageInfo *)NULL);

	(void)strcpy( image_info->filename, str->s );

	Image * image = ReadImage( image_info, &ex );

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

	for ( unsigned short i = 0; i < PixInSquare; ++i )
		hash |= ( (uint_fast64_t)( (unsigned)( mean.red - pixels[i].red ) >> ( sizeof(int) * 8 - 1 ) ) << i );

	img * imagest = malloc(sizeof(img));
	imagest->path = str;
	imagest->hash = hash;
	S_I_add_to_list(imagest);

	DestroyImage(resize_image);

	S_I_check_onexit:
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
I_compare_all ( void )
{
	img * currenthead = List.head;
	for ( size_t i = 0; i + 1 < List.length; ++i )
	{
		img * currenttail = currenthead->next;
		for ( size_t j = i + 1; j < List.length; ++j )
		{
			S_I_compare( currenthead, currenttail );
			currenttail = currenttail->next;
		}

		currenthead = currenthead->next;
	}
}

void
I_init ( void )
{
	PixInSquare = config.avghash_side * config.avghash_side;
}

void
I_process ( string * path )
{
	S_I_check(path);
}
