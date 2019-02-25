#ifndef IMAGELIST_H_GUARD_
#define IMAGELIST_H_GUARD_

/* Macros */
#include "stringutils.h"
#include <stdint.h>
#include <inttypes.h>

/* Typedef */
typedef struct img
{
	string * path;
	uint_fast64_t hash;

	struct img * prev;
	struct img * next;
} img;

typedef struct imagelist
{
	unsigned long length;
	img * head;
	img * tail;
} imagelist;

typedef struct simgroup
{
	uint_fast64_t grouphash;
	imagelist list;
} simgroup;

typedef struct simgrouplist
{
	unsigned long length;
	simgroup * head;
	simgroup * tail;
} simgrouplist;

/* Protos */
void IL_free_imagelist ( imagelist * il );
void IL_add_to_imagelist ( imagelist * il, img * el );

#endif
