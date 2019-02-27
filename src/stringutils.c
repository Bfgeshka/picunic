/* BSD 3-Clause License
 *
 * Copyright (c) 2018-2019, OMG Eto Zhe BFG
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
#include "stringutils.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Local scope */
static void calclen ( string * );

static void
calclen ( string * str )
{
	size_t i = 0;
	for ( ; str->s[i] != '\0'; ++i )
		str->length = i;

	str->length++;
}

/* Global scope */
void
newstring ( string * str, size_t size )
{
	str->bytes = size;
	str->length = 0;
	str->s = malloc(str->bytes);
	str->s[0] = '\0';
}

string *
stringcopy ( string * instr )
{
	string * str = malloc(sizeof(string));
	str->length = instr->length + 1;
	str->bytes = str->length;
	str->s = malloc(str->length);
	snprintf( str->s, str->length, "%s", instr->s );

	return str;
}

string *
construct_string ( size_t size )
{
	string * str = malloc(sizeof(string));
	str->bytes = size;
	str->length = 0;
	str->s = malloc(str->bytes);
	str->s[0] = '\0';

	return str;
}

void
free_string ( string * str )
{
	if ( str == NULL )
		return;

	free(str->s);
	free(str);
}

void
stringset ( string * str, const char * fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	vsnprintf( str->s, str->bytes, fmt, args );
	va_end(args);

	calclen(str);
}

void
stringcat ( string * str, const char * fmt, ... )
{
	string * tmp = construct_string(str->bytes);

	va_list args;
	va_start( args, fmt );
	vsnprintf( tmp->s, tmp->bytes, fmt, args );
	va_end(args);

	strncat( str->s, tmp->s, str->bytes );
	free_string(tmp);
	calclen(str);
}
