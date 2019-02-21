#include "image.h"
#include "stringutils.h"
#include <magick/api.h>
#include <string.h>
#define PICSIZE 32

static void S_I_check ( char * path );

static void
S_I_check ( char * path )
{
	ExceptionInfo exception;
	string * str = construct_string(2048);
	stringset( str, "%s", path );

	InitializeMagick(NULL);
	GetExceptionInfo(&exception);

	ImageInfo * image_info = CloneImageInfo((ImageInfo *)NULL);

	(void)strcpy( image_info->filename, str->s );

	fprintf( stderr, "Reading %s ...", image_info->filename );
	Image * image = ReadImage( image_info, &exception );

	fprintf( stderr, " %lu frames\n", GetImageListLength(image) );
	if ( exception.severity != UndefinedException )
		CatchException(&exception);

	if ( !image )
	{
		fputs( "Null image data.\n", stderr );
		goto S_I_check_onexit;
	}

	Image * resize_image = ResizeImage( image, PICSIZE, PICSIZE, LanczosFilter, 1.0 ,&exception );
	GrayscalePseudoClassImage( resize_image, 1 );

	DestroyImage(image);

	if ( resize_image == (Image *)NULL )
	{
		fputs( "Failed to resize.\n", stderr );
		CatchException(&exception);
		goto S_I_check_onexit;
	}

	stringcat( str, "%s", ".thumb.jpg", image_info->magick );
	(void)strcpy( image_info->filename, str->s );
	FILE * output = fopen( str->s, "w" );

	fprintf( stderr, "Writing %s with res %lux%lu...\n", image_info->filename, resize_image->columns, resize_image->rows );

	if ( !WriteImagesFile( image_info, resize_image, output, &exception ) )
		CatchException(&resize_image->exception);

	fclose(output);
	DestroyImage(resize_image);

	S_I_check_onexit:
	free_string(str);
	DestroyImageInfo(image_info);
	DestroyExceptionInfo(&exception);
	DestroyMagick();
}

void
I_process ( char * path )
{
	S_I_check(path);
}
