#ifndef IMAGE_H_GUARD_
#define IMAGE_H_GUARD_

/* Macros */
#include "stringutils.h"
#include "imagelist.h"

/* Protos */
imgdata * I_process ( string * path );
void I_finish ( void );
void I_init ( char * );

#endif
