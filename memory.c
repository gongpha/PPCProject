#include "memory.h"

#include <stdlib.h>

size_t mem_used = 0;

int mem__create(memory_t* m, size_t byte_num) {
	void* l;
	if (byte_num == 0) {
		l = NULL;
	}
	else {
		l = malloc(byte_num);
		if (!l) return ERR_OUT_OF_MEMORY;
	}

	m->size = byte_num;
	m->size_each = 1;
	m->data = l;

	mem_used += byte_num;
	return ERR_OK;
}

int mem__create_array(memory_t* m, size_t size_each, size_t num) {
	void* l;
	if (num * size_each == 0) {
		l = NULL;
	}
	else {
		l = malloc(num * size_each);
		if (!l) return ERR_OUT_OF_MEMORY;
	}
	

	m->size = num;
	m->size_each = size_each;
	m->data = l;

	mem_used += num * size_each;
	return ERR_OK;
}

void mem__release(memory_t * m) {
	size_t s = m->size * m->size_each;
	if (s == 0) return;
	free(m->data);
	mem_used -= s;
}