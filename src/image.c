/* Macros */
#include "image.h"
#include "imagelist.h"
#include "config.h"
#include "stringutils.h"
#include <magick/api.h>
#include <string.h>

/* Local scope */
static list Images;
static list Simlist;

static unsigned long S_I_get_mean ( PixelPacket * pixels );
static void S_I_compare ( listel * img1, listel * img2 );
static void S_I_hash_from_thumb ( Image * image, string * str );

static void
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

	IL_add_to_list( &Images, imdat );
}

static void
S_I_compare ( listel * img1, listel * img2 )
{
	unsigned similar_bits = config.square;
	imgdata * im1 = (imgdata *)img1->data;
	imgdata * im2 = (imgdata *)img2->data;
	HASHTYPE hash1 = ( im1->group == NULL ) ? im1->hash : im1->group->grhash;
	HASHTYPE hash2 = ( im2->group == NULL ) ? im2->hash : im2->group->grhash;

	HASHTYPE similarity_hash = hash1 ^ hash2;

	if ( im1->group != NULL && im2->group != NULL )
	{
		fprintf( stderr, "Images %" PRIxFAST64 " and %" PRIxFAST64 " both are in groups already, skipping!\n", hash1, hash2 );
		if ( im1->group == im2->group )
			fputs( "They are even in the same group!\n", stderr );
		return;
	}

	fprintf( stderr, "Comparing %" PRIxFAST64 " with %" PRIxFAST64 "... ", hash1, hash2 );

	while ( similarity_hash != 0 )
	{
		similar_bits--;
		similarity_hash &= similarity_hash - 1;
	}
	fprintf( stderr, " similar: %u / %u\n", similar_bits, config.square );

	if ( similar_bits / (float)config.square >= config.precision )
	{
		if ( im1->group == NULL && im2->group == NULL )
		{
			simgroup * grp = malloc(sizeof(grp));
			grp->grhash = 0;
			grp->images.length = 0;
			grp->images.head = NULL;
			grp->images.tail = NULL;

			IL_add_to_simgroup( grp, im1 );
			IL_add_to_simgroup( grp, im2 );
			fputs( "New group!\n", stderr );
			IL_add_to_list( &Simlist, (void *)grp );
		}
		else
		{
			if ( im1->group == NULL )
			{
				fputs( "Adding to im2 group...\n", stderr );
				IL_add_to_simgroup( im2->group, im1 );
			}
			else
			{
				fputs( "Adding to im1 group...\n", stderr );
				IL_add_to_simgroup( im1->group, im2 );
			}
		}
	}
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
I_finish ( void )
{
	IL_free_imagelist(&Images);
}

void
I_compare_all ( void )
{
	listel * currenthead = Images.head;
	size_t i = 0;
	size_t j = i + 1;

	for ( ; i + 1 < Images.length; ++i )
	{
		listel * currenttail = currenthead->next;
		for ( j = i + 1; j < Images.length; ++j )
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
