#include "files.h"
#include "stringutils.h"
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
	string * str = construct_string(4096);

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

		size_t dirsize = 0;
		struct dirent * entry = NULL;
		while ( ( entry = readdir(curdir) ) )
		{
			stringset( str, "%s/%s", curpath, entry->d_name );

			struct stat st;
			if ( stat( str->s, &st ) == 0 && S_ISREG(st.st_mode) )
			{
				dirsize++;
			}
		}

		fprintf( stderr, "Usable files in directory: %zu\n", dirsize );
		closedir(curdir);
	}

	free_string(str);
}
