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
	str->length = instr->length;
	str->bytes = instr->length;
	str->s = malloc(instr->length);
	strcpy( str->s, instr->s );

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

	if ( str != NULL )
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
