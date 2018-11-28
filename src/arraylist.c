//#include <string.h>
#include <mem.h>
#include "error.h"
#include "arraylist.h"

ArrayList *arraylist_create(size_t size) {
	ArrayList *list = NULL;
	void *array = NULL;
	
	TRYNULL(list = malloc(sizeof(ArrayList)), ERR_ERRNO);
	TRYNULL(array = malloc(sizeof(void *) * size), ERR_ERRNO);
	
	list->size = size;
	list->items = 0;
	list->array = array;
	
	return list;
	
	fail:
	free(list);
	free(array);
	return NULL;
}

void arraylist_free(ArrayList *list) {
	if(!list)
		return;
	
	free(list->array);
	free(list);
}

int arraylist_add(ArrayList *list, void *item) {
	if(!list)
		return -1;
	if(list->items == list->size) {
		if(!list->size)
			list->size = 1;
		else
			list->size *= 2;
		TRYNULL(list->array = realloc(list->array, list->size * sizeof(void *)), ERR_ERRNO);
	}
	
	list->array[list->items] = item;
	
	list->items++;
	
	return 0;
	
	fail:
	return -1;
}

int arraylist_insert(ArrayList *list, void *item, size_t index) {
	void **tmp;
	
	if(!list)
		return -1;
	if(index > list->items)
		return -1;
	
	if(index == list->items) {
		return arraylist_add(list, item);
	}
	
	if(list->items == list->size) {
		if(!list->size)
			list->size = 1;
		else
			list->size *= 2;
		TRYNULL(tmp = malloc(list->size * sizeof(void *)), ERR_ERRNO);
		if(index > 0)
			memcpy(tmp, list->array, (index - 1) * sizeof(void *));
		
		tmp[index] = item;
		memcpy(tmp + index + 1, list->array + index, (list->items - index) * sizeof(void *));
		
		free(list->array);
		list->array = tmp;
	} else {
		memmove(list->array + index + 1, list->array + index, (list->items - index) * sizeof(void *));
		list->array[index] = item;
	}
	
	list->items++;
	
	return 0;
	fail:
	return -1;
}

void *arraylist_remove(ArrayList *list, size_t index) {
	void *ret;
	
	if(!list)
		return NULL;
	if(!list->items)
		return NULL;
	
	if(index >= list->items)
		return NULL;
	
	ret = list->array[index];
	if(index == list->items - 1) {
		list->array[index] = NULL;
		list->items--;
		return ret;
	}
	
	memmove(list->array + index, list->array + index + 1, sizeof(void *) * (list->items - index - 1));
	list->items--;
	
	return ret;
}
