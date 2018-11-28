#include <stdint.h>
//#include <string.h>
#include <mem.h>
#include "error.h"
#include "linkedlist.h"

LinkedList *linkedlist_create() {
	LinkedList *list = NULL;
	
	TRYNULL(list = malloc(sizeof(LinkedList)), ERR_ERRNO);
	
	list->items = 0;
	list->first = NULL;
	list->last = NULL;
	
	return list;
	
	fail:
	free(list);
	return NULL;
}

void linkedlist_free(LinkedList *list, void (*foreach_item)(void *)) {
	LinkedListLink *link, *next;
	if(!list)
		return;
	
	for(link = list->first; link; link = next) {
		if(foreach_item)
			foreach_item(link->item);
		next = link->next;
		free(link);
	}
	
	free(list);
}

LinkedListLink *linkedlist_get(LinkedList *list, size_t index) {
	size_t i;
	LinkedListLink *link;
	
	if(!list)
		return NULL;
	
	/*if(index < 0) {
		if(-index > list->items)
			return NULL;
		
		index = -index + 1;
		for(link = list->last, i = list->items; i > index; link = link->prev, i--);
	} else {*/
		if(index > list->items)
			return NULL;
		
		for(link = list->first, i = 0; i < index; link = link->next, i++);
	//}
	
	return link;
	
}

int linkedlist_add(LinkedList *list, void *item) {
	LinkedListLink *link;
	
	if(!list)
		return -1;
	
	TRYNULL(link = malloc(sizeof(LinkedListLink)), ERR_ERRNO);
	link->next = NULL;
	if((link->prev = list->last))
		list->last->next = link;
	if(!list->first)
		list->first = link;
	list->last = link;
	link->item = item;
	
	list->items++;
	
	return 0;
	
	fail:
	return -1;
}

int linkedlist_insert(LinkedList *list, void *item, size_t index) {
	LinkedListLink *link, *newlink;
	
	if(!list)
		return -1;
	
	if(index == list->items)
		return linkedlist_add(list, item);
	
	if(!(link = linkedlist_get(list, index)))
		return -1;
	
	TRYNULL(newlink = malloc(sizeof(LinkedListLink)), ERR_ERRNO);
	newlink->prev = link->prev;
	newlink->next = link;
	newlink->item = item;
	
	if(link->prev)
		link->prev->next = newlink;
	else
		list->first = newlink;
	
	link->prev = newlink;
	list->items++;
	
	return 0;
	fail:
	return -1;
}

void *linkedlist_remove(LinkedList *list, size_t index) {
	LinkedListLink *link;
	void *item;
	
	if(!list)
		return NULL;
	
	if(!(link = linkedlist_get(list, index)))
		return NULL;
	
	if(link->prev)
		link->prev->next = link->next;
	else
		list->first = link->next;
	
	
	if(link->next)
		link->next->prev = link->prev;
	else
		list->last = link->prev;
	
	item = link->item;
	free(link);
	list->items--;
	
	return item;
}

