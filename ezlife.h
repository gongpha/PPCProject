#ifndef _H_EZLIFE_H_
#define _H_EZLIFE_H_

// ezlife(tm)
// make your life better

#define NULL (void*)0x00000000
typedef float number_t;

typedef float scalar_t;
typedef unsigned long u_long;
typedef unsigned short u_short;
typedef unsigned char u_char;

#define ASSET(path) ("assets/" path)

/* fake boolean lmao */
typedef char bool_t;
#define true 1;
#define false 0;

/* STRING METHODS */
#include <string.h>
#define String_MATCH(a, b) (strcmp(a, b) == 0) // "coc" == "kua" ?

/* GLOBAL ERROR CODES */
#define ERR_OK 0
#define ERR_CANNOT_LOAD_RESOURCE	1
#define ERR_RESOURCE_IS_NULL		2
#define ERR_CANNOT_OPEN				3
#define ERR_OUT_OF_MEMORY			555		// why do u laugh ??? your memory is dying apparently

#endif