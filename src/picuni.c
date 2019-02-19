#include "application.h"
#include "files.h"
#include <stdlib.h>
#include <stdio.h>

int
main ( int argc, char ** argv )
{
	A_args( argc, argv );

	F_cook_regexp();
	F_load_all_paths();

	F_finish();
	fputs( "All good, exitting...\n", stderr );
	return EXIT_SUCCESS;
}
