#ifndef _COLLECTIONS_ARRAYLIST_H_
#define _COLLECTIONS_ARRAYLIST_H_

#include <stddef.h>
#include "extratypes.h"

typedef struct ArrayList ArrayList;
struct ArrayList {
	void **array;
	size_t items;
	size_t size;
};

ArrayList *arraylist_create(size_t size);
void arraylist_free(ArrayList *list) ;
int arraylist_add(ArrayList *list, void *item);
int arraylist_insert(ArrayList *list, void *item, size_t index);
void *arraylist_remove(ArrayList *list, size_t index);


#endif
