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
#include "config.h"
#include "files.h"
#include "stringutils.h"
#include "hashprocess.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>

/* Local scope */
regex_t Image_regex;

static void S_F_load_path ( char * path );
static unsigned S_F_get_if_image ( string * path );

static unsigned
S_F_get_if_image ( string * path )
{
	int err_ret = regexec( &Image_regex, path->s, 0, NULL, 0 );
	switch ( err_ret )
	{
		case 0:
		{
			return HP_process(path);
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
	DIR * curdir;
	unsigned long dirsize = 0;
	struct dirent * entry = NULL;
	string * str = construct_string(4096);

	fprintf( stderr, "Loading contents of %s...\n", path );

	if ( ( curdir = opendir(path) ) == NULL )
	{
		fprintf( stderr, "Cannot open %s, skipping...\n", path );
		free_string(str);
		return;
	}

	while ( ( entry = readdir(curdir) ) )
	{
		struct stat st;

		if ( strcmp( entry->d_name, "." ) == 0 || strcmp( entry->d_name, ".." ) == 0 )
			continue;

		stringset( str, "%s/%s", path, entry->d_name );

		if ( stat( str->s, &st ) == 0 )
		{
			if ( S_ISREG(st.st_mode) )
				dirsize += S_F_get_if_image(str);
			if ( config.recursive && S_ISDIR(st.st_mode) )
				S_F_load_path(str->s);
		}
	}

	fprintf( stderr, "Usable files in %s: %lu\n", path, dirsize );
	free_string(str);
	closedir(curdir);
}

/* Global scope */
void
F_check_path ( char * path )
{
	struct stat sb;

	fprintf( stderr, "Checking path %s...\n", path );

	if ( stat( path, &sb ) != 0 || !S_ISDIR(sb.st_mode) )
	{
		fputs( "Invalid path.\n", stderr );
		exit(EXIT_FAILURE);
	}
}

void
F_cook_regexp ( void )
{
	int err_ret = regcomp( &Image_regex, config.regexp, REG_ICASE | REG_EXTENDED | REG_NOSUB );
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
	size_t i = 0;
	for ( ; i < directories.count; ++i )
		S_F_load_path(directories.value[i]);
}
