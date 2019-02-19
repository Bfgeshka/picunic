#include "files.h"
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>


void
F_check_path ( char * path )
{
	fprintf( stderr, "Checking path %s...\n", path );

	struct stat sb;
	if ( stat( path, &sb ) != 0 || !S_ISDIR(sb.st_mode) )
	{
		fputs( "Invalid path.\n", stderr );
		exit(EXIT_FAILURE);
	}
}

void
F_finish ( void )
{
	free(directories.value);
}

void
F_load_all_paths ( void )
{
	for ( size_t i = 0; i < directories.count; ++i )
	{
		char * curpath = directories.value[i];
		fprintf( stderr, "Loading contents of %s...\n", curpath );

		DIR * curdir = opendir(curpath);
		if ( curdir == NULL )
		{
			fprintf( stderr, "Cannot open %s, skipping...\n", curpath );
			continue;
		}

		struct dirent * entry = NULL;
		while ( ( entry = readdir(curdir) ) )
		{
			puts(entry->d_name);
		}

		(void)closedir(curdir);
	}
}
