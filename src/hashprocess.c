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

/* Macros */
#include "image.h"
#include "imagelist.h"
#include "config.h"
#include "application.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Local scope */
static list Images;
static list Simlist;

static void S_HP_compare ( listel * img1, listel * img2 );
static void S_HP_add_to_simgroup ( simgroup * grp, imgdata * im );
static void S_HP_rehash_simgroup ( simgroup * grp );

/* TODO: replace with proper group hash */
static void
S_HP_rehash_simgroup ( simgroup * grp )
{
	grp->grhash = ((imgdata *)grp->images.head->data)->hash;
}

static void
S_HP_add_to_simgroup ( simgroup * grp, imgdata * im )
{
	IL_add_to_simgroup( grp, im );
	S_HP_rehash_simgroup(grp);
}

static void
S_HP_compare ( listel * img1, listel * img2 )
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

			S_HP_add_to_simgroup( grp, im1 );
			S_HP_add_to_simgroup( grp, im2 );
			IL_add_to_list( &Simlist, (void *)grp );
		}
		else
		{
			if ( im1->group == NULL )
				S_HP_add_to_simgroup( im2->group, im1 );
			else
				S_HP_add_to_simgroup( im1->group, im2 );
		}
	}
}

/* Global scope */
void
HP_init ( char * path )
{
	I_init(path);
}

void
HP_finish ( void )
{
	I_finish();

	IL_free_imagelist_payload(&Images);
	IL_free_list(&Images);

	IL_free_simgrouplist_payload(&Simlist);
	IL_free_list(&Simlist);
}

void
HP_compare_all ( void )
{
	unsigned long i = 0;
	unsigned long j = i + 1;
	listel * currenthead = Images.head;

	for ( ; i + 1 < Images.length; ++i )
	{
		listel * currenttail = currenthead->next;
		for ( j = i + 1; j < Images.length; ++j )
		{
			S_HP_compare( currenthead, currenttail );
			currenttail = currenttail->next;
		}

		currenthead = currenthead->next;
	}
}

void
HP_result ( void )
{
	listel * currenthead = Simlist.head;
	unsigned long i = 0;
	unsigned long j = 0;

	for ( ; i < Simlist.length; ++i )
	{
		simgroup * curgroup = (simgroup *)currenthead->data;
		listel * curimg = curgroup->images.head;
		size_t cmdlinesize = ( config.customcmd ) ? strlen(config.cmdline) + 1 : 0;

		fprintf( stderr, "Group %lu/%lu (%" PRIxFAST64 "), %lu images:\n", i + 1, Simlist.length, curgroup->grhash, curgroup->images.length );

		for ( j = 0; j < curgroup->images.length; ++j )
		{
			imgdata * img = (imgdata *)curimg->data;
			fprintf( stderr, "\t%016" PRIxFAST64 "\t%s\n", img->hash, img->path->s );

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

			free_string(str);
		}

		currenthead = currenthead->next;
	}
}

int
HP_process ( string * path )
{
	imgdata * img = I_process(path);

	if ( img != NULL )
	{
		IL_add_to_list( &Images, img );
		return 1;
	}

	return 0;
}
