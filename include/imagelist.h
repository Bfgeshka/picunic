#ifndef IMAGELIST_H_GUARD_
#define IMAGELIST_H_GUARD_

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
 * Gives image-related storage types and functions for helping with lists.
 * */

/* Macros */
#include "stringutils.h"
#include <stdint.h>
#include <inttypes.h>
#define HASHTYPE uint_fast64_t

/* Typedef */
typedef struct imgdata
{
	string * path;
	HASHTYPE hash;
	struct simgroup * group;
} imgdata;

typedef struct listel
{
	void * data;

	struct listel * prev;
	struct listel * next;
} listel;

typedef struct list
{
	unsigned long length;

	listel * head;
	listel * tail;
} list;

typedef struct simgroup
{
	HASHTYPE grhash;
	list images;
} simgroup;

/* Protos */
void IL_add_to_list ( list * ls, void * data );
void IL_free_list ( list * il );
void IL_free_imagelist_payload ( list * il );
void IL_free_simgrouplist_payload ( list * il );
void IL_add_to_simgroup ( simgroup * grp, imgdata * im );

#endif
