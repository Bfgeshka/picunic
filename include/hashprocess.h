#ifndef HASHPROCESS_H_GUARD_
#define HASHPROCESS_H_GUARD_

/* Macros */
#include "stringutils.h"

/* Protos */
int HP_process ( string * path );
void HP_finish ( void );
void HP_compare_all( void );
void HP_result ( void );
void HP_init ( char * );

#endif
