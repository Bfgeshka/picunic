/* Macros */
#include "application.h"
#include "config.h"
#include "files.h"
#include <stdlib.h>
#include <stdio.h>
#define DEFAULT_PRECISION 0.95f
#define DEFAULT_AVGHASH_SIDE 8

/* Global scope */
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
	config.avghash_side = DEFAULT_AVGHASH_SIDE;
	directories.count = 0;

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
				directories.count++;
				break;
			}

			case 'h':
				goto A_args_show_help;

			default:
				goto A_args_invalid_argument;
		}
	}

	if ( directories.count > 0 )
	{
		directories.value = malloc( sizeof(char *) * directories.count );
		int pathindex = 0;

		for ( int i = 1; i < argc; ++i )
		{
			if ( argv[i][0] == '-' && argv[i][1] == 'T' )
			{
				if ( i + 1 == argc )
					goto A_args_invalid_argument;

				F_check_path(argv[i + 1]);
				directories.value[pathindex] = argv[i + 1];
				pathindex++;
			}
		}
	}
	else
	{
		F_check_path(argv[argc - 1]);

		directories.count = 1;
		directories.value = malloc( sizeof(char *) * directories.count );
		directories.value[0] = argv[argc - 1];
	}

	fprintf( stderr, "paths: %zu\nrecursive: %d\nprecision: %f\n", directories.count, config.recursive, config.precision );
	return;

	A_args_invalid_argument:
	fputs( "Invalid argument.\n", stderr );
	exit(EXIT_FAILURE);

	A_args_show_help:
	A_help();
	return;
}
