#ifndef IMAGE_H_
#define IMAGE_H_

/* Macros */
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

/* Typedef */
typedef struct img
{
	char * path;
	uint_fast64_t hash;

	struct img * prev;
	struct img * next;
} img;

typedef struct imagelist
{
	size_t length;
	img * head;
	img * tail;
} imagelist;

/* Protos */
void I_process ( char * path );
void I_stats ( void );
void I_finish ( void );

#endif
