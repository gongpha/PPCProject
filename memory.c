#include "memory.h"

#include "maindef.h"

#include <stdlib.h>

size_t mem_used = 0;

int Mem_create(memory_t* m, size_t byte_num) {
	return Mem_create_array(m, sizeof(char), byte_num);
}

int Mem_create_array(memory_t* m, size_t size_each, size_t num) {
	void* l;
	size_t total = size_each * num;

	if (m->data) {
		if (total == 0) {
			mem_used -= m->size * m->size_each;
			free(m->data);
			l = NULL;
		}
		else {
			mem_used -= m->size * m->size_each;
			mem_used += total;
			l = realloc(m->data, total);
			if (!l) return ERR_OUT_OF_MEMORY;
		}
	}
	else {
		if (total == 0) {
			l = NULL;
		}
		else {
			l = malloc(total);
			if (!l) return ERR_OUT_OF_MEMORY;
		}
	}

	m->size = num;
	m->size_each = size_each;
	m->data = l;

	mem_used += total;
	return ERR_OK;
}

void Mem_release(memory_t * m) {
	if (!m->data) return;
	size_t s = m->size * m->size_each;
	if (s == 0) return;
	free(m->data);
	m->data = NULL;
	mem_used -= s;
}

void Mem_replace(memory_t* from, memory_t* to)
{
	Mem_release(to);
	*to = *from;
}
