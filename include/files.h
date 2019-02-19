#ifndef FILES_H_
#define FILES_H_

#include <stdlib.h>

struct directories
{
	size_t count;
	char ** value;
} directories;

void F_load_all_paths ( void );
void F_finish ( void );
void F_check_path ( char * path );

#endif
