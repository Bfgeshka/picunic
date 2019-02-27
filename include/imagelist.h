#ifndef IMAGELIST_H_GUARD_
#define IMAGELIST_H_GUARD_

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
