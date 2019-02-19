#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

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
void newstring ( string *, size_t );
void stringset ( string *, const char *, ... );
void stringcat ( string *, const char *, ... );
void free_string ( string * );

#endif
