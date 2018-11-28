#ifndef _COLLECTIONS_HASHMAP_H_
#define _COLLECTIONS_HASHMAP_H_

#include <stdint.h>
#include <stddef.h>
#include "extratypes.h"

typedef struct HashMapItem HashMapItem;
struct HashMapItem {
	uint64_t hash;
	void *key;
	void *value;
	size_t keylen;
	HashMapItem *next;
};

typedef struct HashMap HashMap;
struct HashMap {
	size_t items;
	size_t size;
	uint8_t seed[16];
	HashMapItem **array;
	
	int (* compare)(const void *, const void *, size_t);
};

extern uint8_t hashmap_seed[16];

HashMap *hashmap_create(size_t size, int (* compare)(const void *, const void *, size_t));
void hashmap_free(HashMap *map);
void hashmap_free_all(HashMap *map);
int hashmap_resize(HashMap *map, size_t newsize);
/* keylen = 0 means use strlen to find key length */
HashMapItem *hashmap_get(HashMap *map, const void *key, size_t keylen);
/* keylen = 0 means use strlen to find key length */
int hashmap_set(HashMap *map, void *key, size_t keylen, void *value);
int hashmap_remove(HashMap *map, void *key, size_t keylen);

#endif
