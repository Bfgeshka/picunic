#include "application.h"

void
A_help ( void )
{
	puts( "Usage: picunic [OPTIONS] <PATH>" );
	puts( "" );
	puts( "Options:" );
	puts( "-R           recursive" );
	puts( "-p <VALUE>   set precision level " );

	exit(EXIT_SUCCESS);
}

void
A_args ( int argc, char ** argv )
{
	(void)argc;
	(void)argv;
}
