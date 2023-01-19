#ifndef _ALLOC_H_
#define _ALLOC_H_

#define ALLOC(size) alloc(size, __FILE__, __LINE__)

void *alloc(size_t size, char *file, int line);

#endif
