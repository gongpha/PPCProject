#ifndef _H_MEMORY_H_
#define _H_MEMORY_H_

#include "maindef.h"

// memo

typedef struct {
#ifdef DEBUG
	char[4] magic;
#endif
	size_t size;
	size_t size_each;
	void* data;
} memory_t;


int mem__create(memory_t* m, size_t byte_num);
int mem__create_array(memory_t* m, size_t size_each, size_t num);
void mem__release(memory_t* m);

#define MEMCREATE(object, size) if (mem__create(&object, size) == ERR_OUT_OF_MEMORY) return ERR_OUT_OF_MEMORY
#define MEMCREATEARRAY(object, sizeeach, num) if (mem__create_array(&object, sizeeach, num) == ERR_OUT_OF_MEMORY) return ERR_OUT_OF_MEMORY

#define MEMREADLUMP(object, file) fread(object.data, object.size_each, object.size, file)
#define MEMARRAYINDEXPTR(object, cast_to, index) (&((cast_to*)object.data)[index])

#endif