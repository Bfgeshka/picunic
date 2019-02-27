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
 * Gradient hash (default).
 * Image loading implementation based on stb header-only libraries.
 * More info and more portable libs: https://github.com/nothings/stb
 * */

/* Macros */
#include "image.h"
#include "imagelist.h"
#include "config.h"
#include "stringutils.h"

#define STBI_ASSERT(x)
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_BMP
#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "3rdparty/stb_image_resize.h"

/* Local scope */
static uint8_t S_I_get_mean ( uint8_t * pixels );
static imgdata * S_I_ahash ( string * instr );
static imgdata * S_I_ghash ( string * instr );

static imgdata *
S_I_ghash ( string * instr )
{
	int w = -1;
	int h = -1;
	int n = -1;
	uint8_t * data = stbi_load( instr->s, &w, &h, &n, 1 );

	if ( data == NULL )
		return NULL;

	/* TODO: one time allocation */
	uint8_t * resized = malloc(config.square + config.sq_h);
	stbir_resize_uint8( data, w, h, 0, resized, config.sq_w + 1, config.sq_h, 0, 1 );
	free(data);

	imgdata * retvalue = malloc(sizeof(imgdata));
	retvalue->path = stringcopy(instr);
	retvalue->hash = 0;
	retvalue->group = NULL;

	for ( unsigned u = 0, i = 0; u < config.sq_h; ++u )
	{
		for ( unsigned v = 0; v < config.sq_w; ++v, ++i )
			retvalue->hash |= (HASHTYPE)( (unsigned)( resized[i] - resized[i + 1] ) >> ( sizeof(int) * 8 - 1 ) ) << i;
		i++;
	}

	free(resized);
	return retvalue;

}

static imgdata *
S_I_ahash ( string * instr )
{
	int w = -1;
	int h = -1;
	int n = -1;
	uint8_t * data = stbi_load( instr->s, &w, &h, &n, 1 );

	if ( data == NULL )
		return NULL;

	uint8_t * resized = malloc(config.square);
	stbir_resize_uint8( data, w, h, 0, resized, config.sq_w, config.sq_h, 0, 1 );
	free(data);

	uint8_t mean = S_I_get_mean(resized);

	imgdata * retvalue = malloc(sizeof(imgdata));
	retvalue->path = stringcopy(instr);
	retvalue->hash = 0;
	retvalue->group = NULL;

	for ( unsigned i = 0; i < config.square; ++i )
		retvalue->hash |= (HASHTYPE)( (unsigned)( mean - resized[i] ) >> ( sizeof(int) * 8 - 1 ) ) << i;

	free(resized);
	return retvalue;
}

static uint8_t
S_I_get_mean ( uint8_t * pixels )
{
	unsigned long sum = 0;
	unsigned short i = 0;
	for ( ; i < config.square; ++i )
		sum += pixels[i];

	return (uint8_t)( sum / config.square );
}

/* Global scope */
void
I_init ( char * path )
{
	(void)path;
	config.regexp = "(jpe?g?|png)$";
}

void
I_finish ( void )
{
	;
}

imgdata *
I_process ( string * instr )
{
	return S_I_ghash(instr);
}
