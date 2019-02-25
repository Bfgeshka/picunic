#ifndef FILES_H_
#define FILES_H_

/* Macros */
#include <stdlib.h>

/* Typedef */
struct directories
{
	unsigned long count;
	char ** value;
} directories;

/* Protos */
void F_cook_regexp ( void );
void F_load_all_paths ( void );
void F_finish ( void );
void F_check_path ( char * path );

#endif
