#ifndef STRINGUTILS_H_GUARD_
#define STRINGUTILS_H_GUARD_

/* Macros */
#include <stdlib.h>

/* Typedef */
typedef struct string
{
	char * s;
	size_t length;
	size_t bytes;
} string;

/* Protos */
string * construct_string ( size_t );
string * stringcopy ( string * );
void newstring ( string *, size_t );
void stringset ( string *, const char *, ... );
void stringcat ( string *, const char *, ... );
void free_string ( string * );

#endif
