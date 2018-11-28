#include <stdint.h>
#include <string.h>
#include "error.h"
#include "hashmap.h"

/*
 * SipHash, based on reference C implementation by Jean-Philippe Aumasson
 * and Daniel J. Bernstein
 */

/* default: SipHash-2-4 */
#define CROUNDS 2
#define DROUNDS 4

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define U32TO8_LE(p, v) \
	(p)[0] = (uint8_t)((v)); \
	(p)[1] = (uint8_t)((v) >> 8); \
	(p)[2] = (uint8_t)((v) >> 16); \
	(p)[3] = (uint8_t)((v) >> 24);

#define U64TO8_LE(p, v) \
	U32TO8_LE((p), (uint32_t)((v))); \
	U32TO8_LE((p) + 4, (uint32_t)((v) >> 32));

#define U8TO64_LE(p) \
	(((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) | \
	((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | \
	((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | \
	((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56))

#define SIPROUND \
	do { \
		v0 += v1; \
		v1 = ROTL(v1, 13); \
		v1 ^= v0; \
		v0 = ROTL(v0, 32); \
		v2 += v3; \
		v3 = ROTL(v3, 16); \
		v3 ^= v2; \
		v0 += v3; \
		v3 = ROTL(v3, 21); \
		v3 ^= v0; \
		v2 += v1; \
		v1 = ROTL(v1, 17); \
		v1 ^= v2; \
		v2 = ROTL(v2, 32); \
	} while(0)

static uint64_t siphash(const uint8_t *in, uint64_t inlen, const uint8_t *k) {
	/* "somepseudorandomlygeneratedbytes" */
	uint64_t v0 = 0x736f6d6570736575ULL;
	uint64_t v1 = 0x646f72616e646f6dULL;
	uint64_t v2 = 0x6c7967656e657261ULL;
	uint64_t v3 = 0x7465646279746573ULL;
	uint64_t b;
	uint64_t k0 = U8TO64_LE(k);
	uint64_t k1 = U8TO64_LE(k + 8);
	uint64_t m;
	int i;
	const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
	const int left = inlen & 7;
	
	b = ((uint64_t)inlen) << 56;
	v3 ^= k1;
	v2 ^= k0;
	v1 ^= k1;
	v0 ^= k0;

	for (; in != end; in += 8) {
		m = U8TO64_LE(in);
		v3 ^= m;
		
		for (i = 0; i < CROUNDS; ++i)
			SIPROUND;
		
		v0 ^= m;
	}

	switch (left) {
		case 7:
			b |= ((uint64_t)in[6]) << 48;
		case 6:
			b |= ((uint64_t)in[5]) << 40;
		case 5:
			b |= ((uint64_t)in[4]) << 32;
		case 4:
			b |= ((uint64_t)in[3]) << 24;
		case 3:
			b |= ((uint64_t)in[2]) << 16;
		case 2:
			b |= ((uint64_t)in[1]) << 8;
		case 1:
			b |= ((uint64_t)in[0]);
			break;
		case 0:
			break;
	}

	v3 ^= b;
	for (i = 0; i < CROUNDS; ++i)
		SIPROUND;

	v0 ^= b;
	v2 ^= 0xff;
	for (i = 0; i < DROUNDS; ++i)
		SIPROUND;

	b = v0 ^ v1 ^ v2 ^ v3;
	
	return b;
}

/* It is recommended to replace the seed at runtime initialization */
uint8_t hashmap_seed[16] = {
	0x2d, 0x39, 0xad, 0x07, 0xe5, 0xf8, 0xc1, 0x3e,
	0xb3, 0xa7, 0xf8, 0x6a, 0xcc, 0x98, 0xaf, 0x8c,
};

HashMap *hashmap_create(size_t size, int (* compare)(const void *, const void *, size_t)) {
	HashMapItem **array = NULL;
	HashMap *map = NULL;
	size_t actualsize;
	
	if(!compare)
		return NULL;
	
	if(!size)
		actualsize = 16;
	else {
		for(actualsize = 1, size >>= 1; size; size >>= 1)
			actualsize <<= 1;
	}
	
	TRYNULL(map = malloc(sizeof(HashMap)), ERR_ERRNO);
	TRYNULL(array = calloc(actualsize, sizeof(HashMapItem *)), ERR_ERRNO);
	
	map->array = array;
	map->items = 0;
	map->size = actualsize;
	map->compare = compare;
	
	memcpy(map->seed, hashmap_seed, 16);
	
	return map;
	
	fail:
	free(map);
	free(array);
	return NULL;
}

void hashmap_free(HashMap *map) {
	HashMapItem *item, *next;
	size_t i;
	
	if(!map)
		return;
	
	for(i = 0; i < map->size; i++) {
		for(item = map->array[i]; item; item = next) {
			next = item->next;
			free(item);
		}
	}
	
	free(map->array);
	free(map);
}

void hashmap_free_all(HashMap *map) {
	size_t i;
	HashMapItem *item, *next;
	
	if(!map)
		return;
	
	for(i = 0; i < map->size; i++) {
		for(item = map->array[i]; item; item = next) {
			next = item->next;
			free(item->key);
			free(item->value);
			free(item);
		}
	}
	
	free(map->array);
	free(map);
}

int hashmap_resize(HashMap *map, size_t newsize) {
	HashMapItem **array = NULL, *item, *next;
	size_t actualsize, i, newindex;
	
	if(!map)
		return -1;
	
	if(!newsize)
		return -1;
	
	for(actualsize = 1, newsize >>= 1; newsize; newsize >>= 1)
		actualsize <<= 1;

	TRYNULL(array = calloc(actualsize, sizeof(HashMapItem *)), ERR_ERRNO);
	
	for(i = 0; i < map->size; i++) {
		for(item = map->array[i]; item; item = next) {
			next = item->next;
			
			newindex = item->hash & (actualsize - 1);
			
			item->next = array[newindex];
			array[newindex] = item;
		}
	}
	
	free(map->array);
	map->array = array;
	map->size = actualsize;
	return 0;
	
	fail:
	free(array);
	return -1;
}

/* keylen = 0 means use strlen to find key length */
HashMapItem *hashmap_get(HashMap *map, const void *key, size_t keylen) {
	HashMapItem *item;
	uint64_t hash;
	size_t index;
	
	if(!(map && key))
		return NULL;
	
	if(!keylen)
		keylen = strlen(key);
	
	hash = siphash(key, keylen, map->seed);
	index = hash & (map->size - 1);
	
	for(item = map->array[index]; item; item = item->next) {
		if(item->keylen != keylen)
			continue;
		if(!map->compare(key, item->key, keylen))
			break;
	}
	
	return item;
}

/* keylen = 0 means use strlen to find key length */
int hashmap_set(HashMap *map, void *key, size_t keylen, void *value) {
	HashMapItem *item = NULL;
	uint64_t hash;
	size_t index;
	
	if(!(map && key))
		return -1;
	
	TRYNULL(item = malloc(sizeof(HashMapItem)), ERR_ERRNO);
	
	if((map->items + 1) > ((map->size >> 1) + (map->size >> 2)))
		TRY(hashmap_resize(map, map->size << 1), ERR_ERRNO);
	
	if(!keylen)
		keylen = strlen(key);
	
	hash = siphash(key, keylen, map->seed);
	index = hash & (map->size - 1);
	
	item->hash = hash;
	item->key = key;
	item->keylen = keylen;
	item->value = value;
	item->next = map->array[index];
	map->array[index] = item;
	map->items++;
	
	return 0;
	
	fail:
	free(item);
	return -1;
}

int hashmap_remove(HashMap *map, void *key, size_t keylen) {
	HashMapItem **itemp, *item;
	uint64_t hash;
	size_t index;
	
	if(!(map && key))
		return -1;
	
	if(!keylen)
		keylen = strlen(key);
	
	hash = siphash(key, keylen, map->seed);
	index = hash & (map->size - 1);
	
	for(itemp = &map->array[index]; *itemp; itemp = &((*itemp)->next)) {
		item = *itemp;
		if(item->keylen != keylen)
			continue;
		if(!map->compare(key, item->key, keylen)) {
			*itemp = item->next;
			free(item);
			map->items--;
			//TODO: free key/value?
			return 0;
		}
	}
	
	return -1;
}
