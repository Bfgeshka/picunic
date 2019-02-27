/* BSD 3-Clause License
 *
 * Copyright (c) 2019, OMG Eto Zhe BFG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* * About * *
 * Mean hash.
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
	config.regexp = "(jpe?g?|gif|png)$";
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
