#ifndef _ERROR_H_
#define _ERROR_H_

//#include <errno.h>
#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

/* Messages for error handling */
//special message using errno
#define ERR_ERRNO LOG_LEVEL_ERROR, "in %s:%i: %s\n", __FILE__, __LINE__, strerror(errno)
//special message for backtrace
#define NOTE_LINE LOG_LEVEL_NOTE, "from %s:%i\n", __FILE__, __LINE__

/* Error handling macros */

/*
#define TRYFATAL(x, ...) do {if((x) < 0) { \
	log_printf(__VA_ARGS__); \
	exit(1); \
}} while(0)

#define TRYNULLFATAL(x, ...) do {if((x) == NULL) { \
	log_printf( __VA_ARGS__); \
	exit(1); \
}} while(0)

#define TRY(x, ...) do {if((x) < 0) { \
	log_printf(__VA_ARGS__); \
	goto fail; \
}} while(0)

#define TRYNULL(x, ...) do {if((x) == NULL) { \
	log_printf(__VA_ARGS__); \
	goto fail; \
}}while(0)
*/

#define TRY(x, ...) do {if((x) < 0) { \
	goto fail; \
}} while(0)

#define TRYNULL(x, ...) do {if((x) == NULL) { \
	goto fail; \
}}while(0)

#endif
