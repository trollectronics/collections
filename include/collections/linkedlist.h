#ifndef _COLLECTIONS_LINKEDLIST_
#define _COLLECTIONS_LINKEDLIST_

#include <stdint.h>
#include <stdlib.h>

typedef struct LinkedListLink LinkedListLink;
struct LinkedListLink {
	void *item;
	LinkedListLink *next;
	LinkedListLink *prev;
};

typedef struct LinkedList LinkedList;
struct LinkedList {
	LinkedListLink *first;
	LinkedListLink *last;
	size_t items;
};

LinkedList *linkedlist_create();
void linkedlist_free(LinkedList *list, void (*foreach_item)(void *));
LinkedListLink *linkedlist_get(LinkedList *list, size_t index);
int linkedlist_add(LinkedList *list, void *item);
int linkedlist_insert(LinkedList *list, void *item, size_t index);
void *linkedlist_remove(LinkedList *list, size_t index);

#endif
