#include "application.h"
#include "files.h"
#include "image.h"
#include <stdlib.h>
#include <stdio.h>

int
main ( int argc, char ** argv )
{
	A_args( argc, argv );
	F_cook_regexp();
	I_init(argv[0]);

	F_load_all_paths();
	I_compare_all();
	I_result();

	I_finish();
	F_finish();
	fputs( "All good, exitting...\n", stderr );
	return EXIT_SUCCESS;
}
