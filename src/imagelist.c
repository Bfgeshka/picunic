/* Macros */
#include "imagelist.h"
#include "stringutils.h"

/* Local scope */
static void S_IL_free_imgdata ( void * data );
static void S_IL_rehash_simgroup ( simgroup * grp );
static void S_IL_free_simgroup ( void * data );

static void
S_IL_rehash_simgroup ( simgroup * grp )
{
	grp->grhash = ((imgdata *)grp->images.head->data)->hash;
}

static void
S_IL_free_imgdata ( void * data )
{
	free_string(((imgdata *)data)->path);
	free(data);
}

static void
S_IL_free_simgroup ( void * data )
{
	IL_free_list(&((simgroup *)data)->images);
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
IL_free_imagelist_payload ( list * il )
{
	unsigned long i = 0;
	listel * current = il->head;
	for ( ; i < il->length; ++i )
	{
		S_IL_free_imgdata(current->data);
		current = current->next;
	}
}

void
IL_free_simgrouplist_payload ( list * il )
{
	unsigned long i = 0;
	listel * current = il->head;
	for ( ; i < il->length; ++i )
	{
		S_IL_free_simgroup(current->data);
		current = current->next;
	}
}

void
IL_free_list ( list * il )
{
	unsigned long i = 0;
	for ( ; i < il->length; ++i )
	{
		listel * next = il->head->next;
		free(il->head);
		il->head = next;
	}
}

void
IL_add_to_simgroup ( simgroup * grp, imgdata * im )
{
	IL_add_to_list( &(grp->images), (void *)im );
	im->group = grp;
	S_IL_rehash_simgroup(grp);
}
