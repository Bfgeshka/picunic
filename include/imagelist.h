#ifndef IMAGELIST_H_GUARD_
#define IMAGELIST_H_GUARD_

/* Macros */
#include "stringutils.h"
#include <stdint.h>
#include <inttypes.h>

/* Typedef */
typedef struct imgdata
{
	string * path;
	uint_fast64_t hash;
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
	uint_fast64_t grhash;
	list * images;
} simgroup;

/* Protos */
void IL_add_to_list ( list * ls, void * data );
void IL_free_imagelist ( list * il );
#endif
