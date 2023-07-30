#ifndef _H_MEMORY_H_
#define _H_MEMORY_H_



// memo

typedef struct {
	size_t size;
	size_t size_each;
	void* data;
} memory_t;


int Mem_create(memory_t* m, size_t byte_num);
int Mem_create_array(memory_t* m, size_t size_each, size_t num);
void Mem_release(memory_t* m);

void Mem_replace(memory_t* from, memory_t* to);

#define MEMSTATIC(name) memory_t name = { 0, 0, NULL }
#define MEM(name) name = (memory_t){ 0, 0, NULL }

#define MEMCREATE(object, size) if (Mem_create(&object, size) == ERR_OUT_OF_MEMORY) return ERR_OUT_OF_MEMORY
#define MEMCREATEARRAY(object, sizeeach, num) if (Mem_create_array(&object, sizeeach, num) == ERR_OUT_OF_MEMORY) return ERR_OUT_OF_MEMORY
#define MEMCREATEARRAYLUMP(object, sizeeach, sizetotal) MEMCREATEARRAY(object, sizeeach, sizetotal / sizeeach)

#define MEMREADLUMP(object, file) fread((char*)object.data, object.size_each, object.size, file)
#define MEMARRAYINDEXPTR(object, cast_to, index) (&((cast_to*)object.data)[index])

#define MEMCREATEDYNAMICARRAY(object, type) MEMCREATEARRAY(object, sizeof(type), 2)
#define MEMDYNAMICARRAYPUSH(object, type, counter, item) { if (counter >= object.size) MEMCREATEARRAY(object, sizeof(type), object.size * 2); *MEMARRAYINDEXPTR(object, type, counter++) = item; }
//#define MEMDYNAMICARRAYPUSHN(object, type, counter, n, items) { while (counter + (n - 1) >= object.size) MEMCREATEARRAY(object, sizeof(type), object.size * 2); memcpy(MEMARRAYINDEXPTR(object, type, counter), items, n); counter += n; }

#endif