#include "application.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#define DEFAULT_PRECISION 0.95f

void
A_help ( void )
{
	puts("Usage: picunic [OPTIONS] <PATH>");
	puts("");
	puts("Options:");
	puts("    -R           recursive");
	puts("    -p <VALUE>   set precision level, between 0 and 1 (not inclusive)");
	puts("    -h           show this help message");

	exit(EXIT_SUCCESS);
}

void
A_args ( int argc, char ** argv )
{
	if ( argc == 1 )
		goto A_args_show_help;

	config.precision = DEFAULT_PRECISION;

	for ( int i = 1; i + 1 < argc; ++i )
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
				float tmp = atof(argv[i+1]);
				if ( tmp <= 0.0f || tmp >= 1.0f )
					goto A_args_invalid_argument;

				config.precision = tmp;
				break;
			}

			default:
				goto A_args_invalid_argument;
		}
	}

	config.path = argv[argc - 1];

	struct stat sb;
	if ( stat( config.path, &sb ) != 0 || !S_ISDIR(sb.st_mode) )
	{
		fputs( "Invalid path.\n", stderr );
		exit(EXIT_FAILURE);
	}

	printf( "path: %s\nrecursive: %d\nprecision: %f\n", config.path, config.recursive, config.precision );
	return;

	A_args_invalid_argument:
	fputs( "Invalid argument.\n", stderr );
	exit(EXIT_FAILURE);

	A_args_show_help:
	A_help();
	return;
}
