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


int Mem_create(memory_t* m, size_t byte_num);
int Mem_create_array(memory_t* m, size_t size_each, size_t num);
void Mem_release(memory_t* m);

void Mem_replace(memory_t* from, memory_t* to);

#define MEMSTATIC(name) memory_t name = { 0, 0, NULL }
#define MEM(name) {name.size = 0; name.size_each = 0; name.data = NULL;}

#define MEMCREATE(object, size) if (Mem_create(&object, size) == ERR_OUT_OF_MEMORY) return ERR_OUT_OF_MEMORY
#define MEMCREATEARRAY(object, sizeeach, num) if (Mem_create_array(&object, sizeeach, num) == ERR_OUT_OF_MEMORY) return ERR_OUT_OF_MEMORY

#define MEMREADLUMP(object, file) fread(object.data, object.size_each, object.size, file)
#define MEMARRAYINDEXPTR(object, cast_to, index) (&((cast_to*)object.data)[index])

#endif