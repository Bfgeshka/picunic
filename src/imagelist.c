/* Macros */
#include "imagelist.h"
#include "stringutils.h"

/* Local scope */
static void S_IL_free_imgdata ( void * data );

static void
S_IL_free_imgdata ( void * data )
{
	free_string(((imgdata *)data)->path);
	free(data);
}

/* Global scope */
void
IL_add_to_list ( list * ls, void * data )
{
	listel * el = malloc(sizeof(listel));
	el->data = data;

	if ( ls->length == 0 )
	{
		ls->length = 1;
		ls->head = el;
		ls->tail = el;
		return;
	}

	{
		listel * curtail = ls->tail;
		curtail->next = el;
		el->prev = curtail;
		ls->tail = el;
		ls->length++;
	}
}

void
IL_free_imagelist ( list * il )
{
	unsigned long i = 0;
	for ( ; i + 1 < il->length; ++i )
	{
		listel * next = il->head->next;
		S_IL_free_imgdata(il->head->data);
		free(il->head);
		il->head = next;
	}
}
