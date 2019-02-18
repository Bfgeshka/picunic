#include "application.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#define DEFAULT_PRECISION 0.95f

static void S_A_check_path ( char * path );

static void
S_A_check_path ( char * path )
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
A_help ( void )
{
	puts("Usage:");
	puts("    picunic [OPTIONS] <PATH>");
	puts("    picunic [OPTIONS] -T <PATH> [-T <PATH>]");
	puts("");
	puts("Options:");
	puts("    -R           recursive");
	puts("    -p <VALUE>   set precision level, between 0 and 1 (not inclusive)");
	puts("    -h           show this help message");
	puts("    -T <PATH>    set path");

	exit(EXIT_SUCCESS);
}

void
A_args ( int argc, char ** argv )
{
	if ( argc == 1 )
		goto A_args_show_help;

	config.precision = DEFAULT_PRECISION;
	config.pathnumber = 0;

	for ( int i = 1; i < argc; ++i )
	{
		if ( argv[i][0] != '-' )
			continue;

		switch ( argv[i][1] )
		{
			case 'R':
			{
				config.recursive = 1;
				break;
			}

			case 'p':
			{
				if ( i + 1 == argc )
					goto A_args_invalid_argument;

				float tmp = atof(argv[i + 1]);
				if ( tmp <= 0.0f || tmp >= 1.0f )
					goto A_args_invalid_argument;

				config.precision = tmp;
				break;
			}

			case 'T':
			{
				config.pathnumber++;
				break;
			}

			case 'h':
				goto A_args_show_help;

			default:
				goto A_args_invalid_argument;
		}
	}

	if ( config.pathnumber > 0 )
	{
		config.path = malloc( sizeof(char *) * config.pathnumber );
		int pathindex = 0;

		for ( int i = 1; i < argc; ++i )
		{
			if ( argv[i][0] == '-' && argv[i][1] == 'T' )
			{
				if ( i + 1 == argc )
					goto A_args_invalid_argument;

				S_A_check_path(argv[i + 1]);
				config.path[pathindex] = argv[i + 1];
				pathindex++;
			}
		}
	}
	else
	{
		S_A_check_path(argv[argc - 1]);

		config.pathnumber = 1;
		config.path = malloc( sizeof(char *) * config.pathnumber );
		config.path[0] = argv[argc - 1];
	}

//	config.path = argv[argc - 1];

	printf( "paths: %d\nrecursive: %d\nprecision: %f\n", config.pathnumber, config.recursive, config.precision );
	return;

	A_args_invalid_argument:
	fputs( "Invalid argument.\n", stderr );
	exit(EXIT_FAILURE);

	A_args_show_help:
	A_help();
	return;
}
