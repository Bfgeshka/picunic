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
#include "imagelist.h"
#include "stringutils.h"

/* Local scope */
static void S_IL_free_imgdata ( void * data );
static void S_IL_free_simgroup ( void * data );

static void
S_IL_free_imgdata ( void * data )
{
	free_string(((imgdata *)data)->path);
	free(data);
}

static void
S_IL_free_simgroup ( void * data )
{
	IL_free_list(&((simgroup *)data)->images);
	free(data);
}

/* Global scope */
void
IL_add_to_list ( list * ls, void * data )
{
	listel * el = malloc(sizeof(listel));
	el->data = data;

	if ( ls->length == 0 )
	{
		ls->length = 1;
		ls->head = el;
		ls->tail = el;
		return;
	}

	{
		listel * curtail = ls->tail;
		curtail->next = el;
		el->prev = curtail;
		ls->tail = el;
		ls->length++;
	}
}

void
IL_free_imagelist_payload ( list * il )
{
	unsigned long i = 0;
	listel * current = il->head;
	for ( ; i < il->length; ++i )
	{
		S_IL_free_imgdata(current->data);
		current = current->next;
	}
}

void
IL_free_simgrouplist_payload ( list * il )
{
	unsigned long i = 0;
	listel * current = il->head;
	for ( ; i < il->length; ++i )
	{
		S_IL_free_simgroup(current->data);
		current = current->next;
	}
}

void
IL_free_list ( list * il )
{
	unsigned long i = 0;
	for ( ; i < il->length; ++i )
	{
		listel * next = il->head->next;
		free(il->head);
		il->head = next;
	}
}

void
IL_add_to_simgroup ( simgroup * grp, imgdata * im )
{
	IL_add_to_list( &(grp->images), (void *)im );
	im->group = grp;
}
