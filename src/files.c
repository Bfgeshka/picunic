/* Macros */
#include "config.h"
#include "files.h"
#include "stringutils.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Local scope */
static void S_F_load_path ( char * path );

static void S_F_load_path ( char * path )
{
		fprintf( stderr, "Loading contents of %s...\n", path );

		DIR * curdir = opendir(path);
		if ( curdir == NULL )
		{
			fprintf( stderr, "Cannot open %s, skipping...\n", path );
			return;
		}

		size_t dirsize = 0;
		struct dirent * entry = NULL;
		string * str = construct_string(4096);

		while ( ( entry = readdir(curdir) ) )
		{
			if ( strcmp( entry->d_name, "." ) == 0 || strcmp( entry->d_name, ".." ) == 0 )
				continue;

			stringset( str, "%s/%s", path, entry->d_name );

			struct stat st;
			if ( stat( str->s, &st ) == 0 )
			{
				if ( S_ISREG(st.st_mode) )
					dirsize++;

				if ( config.recursive && S_ISDIR(st.st_mode) )
					S_F_load_path(str->s);
			}
		}

		fprintf( stderr, "Usable files in %s: %zu\n", path, dirsize );
		free_string(str);
		closedir(curdir);
}

/* Global scope */
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
		S_F_load_path(directories.value[i]);
}
