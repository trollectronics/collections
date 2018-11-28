//#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
//#include <string.h>
#include <stdarg.h>
#include <mem.h>
#include "error.h"
#include "stringbuilder.h"

StringBuilder *stringbuilder_create(size_t size) {
	StringBuilder *builder = NULL;
	void *string = NULL;
	
	TRYNULL(builder = malloc(sizeof(StringBuilder)), ERR_ERRNO);
	TRYNULL(string = malloc(sizeof(char) * size), ERR_ERRNO);
	
	builder->size = size;
	builder->length = 0;
	builder->string = string;
	
	return builder;
	
	fail:
	free(string);
	free(builder);
	return NULL;
}

void stringbuilder_free(StringBuilder *builder) {
	if(!builder)
		return;
	
	free(builder->string);
	free(builder);
}

char *stringbuilder_terminate(StringBuilder *builder, size_t *length, size_t *size) {
	char *string;
	
	if(!builder)
		return NULL;
	
	if(length)
		*length = builder->length;
	
	if(stringbuilder_append_char(builder, 0) < 0)
		goto fail;
	if(size)
		*size = builder->size;
	string = builder->string;
	
	return string;
	
	fail:
	return NULL;
}

char *stringbuilder_finish(StringBuilder *builder, size_t *length, size_t *size) {
	char *string;
	
	if(!(string = stringbuilder_terminate(builder, length, size)))
		return NULL;
	
	free(builder);
	return string;
}

int stringbuilder_append_char(StringBuilder *builder, char c) {
	char *tmp;
	
	if(!builder)
		return -1;
	
	if(builder->length == builder->size) {
		if(!builder->size)
			builder->size = 1;
		else
			builder->size *= 2;
		TRYNULL(tmp = realloc(builder->string, builder->size), ERR_ERRNO);
		builder->string = tmp;
	}
	
	builder->string[builder->length] = c;
	
	builder->length++;
	return 0;
	
	fail:
	return -1;
}

ssize_t stringbuilder_append_string(StringBuilder *builder, const char *str) {
	size_t i;
	if(!(builder && str))
		return -1;
	
	for(i = 0; *str; str++, i++)
		if(stringbuilder_append_char(builder, *str) < 0)
			return -1;
	
	return i;
}

ssize_t stringbuilder_append_stringn(StringBuilder *builder, const char *str, size_t n) {
	size_t i;
	if(!(builder && str))
		return -1;
	
	for(i = 0; i < n && *str; str++, i++)
		if(stringbuilder_append_char(builder, *str) < 0)
			return -1;
	
	return i;
}


ssize_t stringbuilder_append_raw(StringBuilder *builder, const char *str, size_t n) {
	size_t i;
	if(!(builder && str))
		return -1;
	
	for(i = 0; i < n; str++, i++)
		if(stringbuilder_append_char(builder, *str) < 0)
			return -1;
	
	return i;
}

/*ssize_t stringbuilder_append_format(StringBuilder *builder, const char *format, ...) {
	ssize_t ret;
	char *tmp;
	bool do_realloc;
	va_list va;
	if(!(builder && format))
		return -1;
	
	va_start(va, format);
	
	ret = vsnprintf(NULL, 0, format, va);
	do_realloc = false;
	while(builder->length + ret + 1 > builder->size) {
		if(!builder->size)
			builder->size = 1;
		else
			builder->size *= 2;
		
		do_realloc = true;
	}
	
	if(do_realloc) {
		TRYNULL(tmp = realloc(builder->string, builder->size), ERR_ERRNO);
		builder->string = tmp;
	}
	
	ret = vsprintf(builder->string + builder->length, format, va);
	builder->length += ret;
	
	va_end(va);
	return ret;
	
	fail:
	return -1;
}*/

ssize_t stringbuilder_erase(StringBuilder *builder, size_t n) {
	if(!builder)
		return -1;
	if(n > builder->length)
		n = builder->length;
	builder->length -= n;
	return n;
}

ssize_t stringbuilder_reset(StringBuilder *builder) {
	size_t ret;
	if(!builder)
		return -1;
	ret = builder->length;
	builder->length = 0;
	return ret;
}
