#include "imagelist.h"
#include "stringutils.h"

void
IL_free_imagelist ( imagelist * il )
{
	unsigned long i = 0;
	for ( ; i < il->length; ++i )
	{
		img * next = il->head->next;
		free_string(il->head->path);
		free(il->head);
		il->head = next;
	}
}

void
IL_add_to_imagelist ( imagelist * il, img * el )
{
	if ( il->length == 0 )
	{
		il->length++;
		il->head = el;
		il->tail = el;
		return;
	}

	il->tail->next = el;
	il->tail = el;
	il->length++;
}
