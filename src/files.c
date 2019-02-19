/* Macros */
#include "config.h"
#include "files.h"
#include "stringutils.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>

/* Local scope */
regex_t Image_regex;

static void S_F_load_path ( char * path );
static int S_F_get_if_image ( char * path );

static int
S_F_get_if_image ( char * path )
{
	int err_ret = regexec( &Image_regex, path, 0, NULL, 0 );
	switch ( err_ret )
	{
		case 0:
		{
			fprintf( stderr, "Match: %s\n", path );
			return 1;
		}
		case REG_NOMATCH:
		{
			return 0;
		}
		default:
		{
			fputs( "regex error.", stderr );
			return 0;
		}
	}

}

static void
S_F_load_path ( char * path )
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
					dirsize += S_F_get_if_image(entry->d_name);

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
F_cook_regexp ( void )
{
	int err_ret = regcomp( &Image_regex, "gif|png|jpg$", REG_ICASE | REG_EXTENDED | REG_NOSUB );
	if ( err_ret != 0 )
	{
		fputs( "Failed to compile regexp, exitting...", stderr );
		exit(EXIT_FAILURE);
	}
}

void
F_finish ( void )
{
	free(directories.value);
	regfree(&Image_regex);
}

void
F_load_all_paths ( void )
{
	for ( size_t i = 0; i < directories.count; ++i )
		S_F_load_path(directories.value[i]);
}
