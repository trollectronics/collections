#ifndef _COLLECTIONS_STRINGBUILDER_H_
#define _COLLECTIONS_STRINGBUILDER_H_

#include <stddef.h>

typedef struct StringBuilder StringBuilder;
struct StringBuilder {
	char *string;
	unsigned int length;
	unsigned int size;
};

StringBuilder *stringbuilder_create(size_t size);
void stringbuilder_free(StringBuilder *builder);
char *stringbuilder_terminate(StringBuilder *builder, size_t *length, size_t *size);
char *stringbuilder_finish(StringBuilder *builder, size_t *length, size_t *size);
int stringbuilder_append_char(StringBuilder *builder, char c);
ssize_t stringbuilder_append_string(StringBuilder *builder, const char *str);
ssize_t stringbuilder_append_stringn(StringBuilder *builder, const char *str, size_t n);
ssize_t stringbuilder_append_raw(StringBuilder *builder, const char *str, size_t n);
ssize_t stringbuilder_append_format(StringBuilder *builder, const char *format, ...);
ssize_t stringbuilder_erase(StringBuilder *builder, size_t n);
ssize_t stringbuilder_reset(StringBuilder *builder);

#endif
