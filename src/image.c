#include "image.h"
#include "stringutils.h"
#include <magick/api.h>
#include <string.h>

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

	Image * resize_image = ResizeImage( image, 32, 32, LanczosFilter, 1.0 ,&exception );
	DestroyImage(image);

	if ( resize_image == (Image *)NULL )
	{
		fputs( "Failed to resize.\n", stderr );
		CatchException(&exception);
		goto S_I_check_onexit;
	}

	stringcat( str, "%s", ".thumb.jpg" );
	(void)strcpy( image_info->filename, str->s );

	fprintf( stderr, "Writing %s ...\n", image_info->filename );
	WriteImage( image_info, resize_image );

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
