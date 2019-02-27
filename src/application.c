/* BSD 3-Clause License
 *
 * Copyright (c) 2019, OMG Eto Zhe BFG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
A_custom_command ( char * line )
{
	fprintf( stderr, "Calling %s...\n", line );
	int z = system(line);
	fprintf( stderr, "system(%s) finished with status %d\n", line, z );
}

void
A_help ( void )
{
	puts("Usage:");
	puts("    picunic [OPTIONS] <PATH>");
	puts("    picunic [OPTIONS] -T <PATH> [-T <PATH>] [OPTIONS]");
	puts("");
	puts("Options:");
	puts("    -R                recursive");
	puts("    -p <VALUE>        set precision level, between 0 and 1 (not inclusive)");
	puts("    -h                show this help message");
	puts("    -T <PATH>         set path");
	puts("    -c \"COMMANDLINE\"  launch COMMANDLINE for each group of similar images");

	exit(EXIT_SUCCESS);
}

void
A_args ( int argc, char ** argv )
{
	int i = 1;
	config.precision = DEFAULT_PRECISION;
	config.sq_w = DEFAULT_AVGHASH_SIDE;
	config.sq_h = DEFAULT_AVGHASH_SIDE;
	config.square = config.sq_w * config.sq_h;
	config.recursive = 0;
	config.customcmd = 0;
	directories.count = 0;

	if ( argc == 1 )
		goto A_args_show_help;

	for ( ; i < argc; ++i )
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
				float tmp;

				if ( i + 1 == argc )
					goto A_args_invalid_argument;

				tmp = atof(argv[i + 1]);
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

			case 'c':
			{
				config.customcmd = 1;
				config.cmdline = argv[i + 1];

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
		int pathindex = 0;
		int u = 1;
		directories.value = malloc( sizeof(char *) * directories.count );

		for ( ; u < argc; ++u )
			if ( argv[u][0] == '-' && argv[u][1] == 'T' )
			{
				if ( u + 1 == argc )
					goto A_args_invalid_argument;

				F_check_path(argv[u + 1]);
				directories.value[pathindex] = argv[u + 1];
				pathindex++;
			}
	}
	else
	{
		F_check_path(argv[argc - 1]);

		directories.count = 1;
		directories.value = malloc( sizeof(char *) * directories.count );
		directories.value[0] = argv[argc - 1];
	}

	fprintf( stderr, "paths: %lu; recursive: %u; precision: %.3f\n", directories.count, config.recursive, config.precision );
	return;

	A_args_invalid_argument:
	fputs( "Invalid argument.\n", stderr );
	exit(EXIT_FAILURE);

	A_args_show_help:
	A_help();
	return;
}
