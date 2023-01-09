//
// Created by lianyu on 2023/1/10.
//

#include "aligned_alloc.h"

#if defined(__APPLE__) || defined(__linux__)

#include <stdlib.h>

void *aligned_malloc(size_t size, size_t alignment) {
  void *pointer = NULL;
  int rv = posix_memalign(&pointer, alignment, size);
  if (rv != 0) {
    pointer = NULL;
  }
  return pointer;
}

void aligned_free(void *pointer) {
  free(pointer);
}

#else

#include <stdlib.h>

void *aligned_malloc(size_t size, size_t alignment) {
	void *p1; // original block
	void **p2; // aligned block
	int offset = alignment - 1 + sizeof(void *);
	if ((p1 = (void *)malloc(size + offset)) == NULL)
		return NULL;
	p2 = (void **)(((size_t)(p1) + offset) & ~(alignment - 1));
	p2[-1] = p1;
	return p2;
}

void aligned_free(void *pointer) {
	free(((void **)pointer)[-1]);
}

#endif
