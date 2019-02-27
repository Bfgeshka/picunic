/* Macros */
#include "image.h"
#include "imagelist.h"
#include "config.h"
#include "stringutils.h"
#include "application.h"
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
		return;

	while ( similarity_hash != 0 )
	{
		similar_bits--;
		similarity_hash &= similarity_hash - 1;
	}

	if ( similar_bits / (float)config.square >= config.precision )
	{
		if ( im1->group == NULL && im2->group == NULL )
		{
			simgroup * grp = malloc(sizeof(simgroup));
			grp->grhash = 0;
			grp->images.length = 0;
			grp->images.head = NULL;
			grp->images.tail = NULL;

			IL_add_to_simgroup( grp, im1 );
			IL_add_to_simgroup( grp, im2 );
			IL_add_to_list( &Simlist, (void *)grp );
		}
		else
		{
			if ( im1->group == NULL )
				IL_add_to_simgroup( im2->group, im1 );
			else
				IL_add_to_simgroup( im1->group, im2 );
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
	DestroyMagick();
}

void
I_init ( char * path )
{
	InitializeMagick(path);
}

void
I_compare_all ( void )
{
	listel * currenthead = Images.head;
	unsigned long i = 0;
	unsigned long j = i + 1;

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
I_result ( void )
{
	listel * currenthead = Simlist.head;
	unsigned long i = 0;
	unsigned long j = 0;

	for ( ; i < Simlist.length; ++i )
	{
		simgroup * curgroup = (simgroup *)currenthead->data;
		listel * curimg = curgroup->images.head;
		size_t cmdlinesize = strlen(config.cmdline) + 1;

		fprintf( stderr, "Group %lu/%lu (%" PRIxFAST64 "), %lu images:\n", i + 1, Simlist.length, curgroup->grhash, curgroup->images.length );

		for ( j = 0; j < curgroup->images.length; ++j )
		{
			imgdata * img = (imgdata *)curimg->data;
			fprintf( stderr, "\t%" PRIxFAST64 "\t%s\n", img->hash, img->path->s );

			cmdlinesize += img->path->length + 3;

			curimg = curimg->next;
		}

		if ( config.customcmd )
		{
			string * str = construct_string(cmdlinesize);
			curimg = curgroup->images.head;

			stringcat( str, "%s ", config.cmdline );

			for ( j = 0; j < curgroup->images.length; ++j )
			{
				stringcat( str, "\"%s\" ", ((imgdata *)curimg->data)->path->s );
				curimg = curimg->next;
			}

			A_custom_command(str->s);

/* 			free_string(str); TODO: fix this */
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
}
