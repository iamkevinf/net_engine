#ifndef __MEM_ALLOCTOR_H__
#define __MEM_ALLOCTOR_H__

void* operator new(size_t size);
void* operator new[](size_t size);

void operator delete(void* p);
void operator delete[](void* p);

void* mem_alloc(size_t size);
void mem_free(void* p);

#endif // __MEM_ALLOCTOR_H__