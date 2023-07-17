#ifndef _H_EZLIFE_H_
#define _H_EZLIFE_H_

// ezlife(tm)
// make your life better

#define NULL (void*)0x00000000
typedef float number_t;

/* fake boolean lmao */
typedef char bool_t;
#define true 1;
#define false 0;

/* STRING METHODS */
#include <string.h>
#define String_MATCH(a, b) (strcmp(a, b) == 0) // "coc" == "kua" ?

#endif