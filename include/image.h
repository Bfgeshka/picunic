#ifndef IMAGE_H_
#define IMAGE_H_

/* Macros */
#include "stringutils.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

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
void I_process ( string * path );
void I_stats ( void );
void I_finish ( void );
void I_compare_all( void );

#endif
