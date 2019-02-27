#include "application.h"
#include "files.h"
#include "hashprocess.h"
#include <stdlib.h>
#include <stdio.h>

int
main ( int argc, char ** argv )
{
	A_args( argc, argv );
	F_cook_regexp();
	HP_init(argv[0]);

	F_load_all_paths();
	HP_compare_all();
	HP_result();

	HP_finish();
	F_finish();
	fputs( "All good, exitting...\n", stderr );
	return EXIT_SUCCESS;
}
