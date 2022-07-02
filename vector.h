#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdarg.h>
#include <stdint.h>

#define CAPACITY_TYPE uint32_t

typedef void *vector_t;
typedef void (*vec_dtor)(void *e);
typedef CAPACITY_TYPE pos_t;

vector_t vec_init(vec_dtor dtor);
void vec_uninit(vector_t vector);

int vec_push_back(vector_t vector, void *e);
int vec_insert(vector_t vector, void *e, pos_t pos);
void vec_erase(vector_t vector, pos_t pos);
void *vec_at(vector_t vector, pos_t pos);
pos_t vec_size(vector_t vector);
pos_t vec_capacity(vector_t vector);
pos_t vec_for_each(vector_t vector, int (*func)(void *e, va_list va),
	...);
void **vec_idx2addr(vector_t vector, pos_t idx);

#define VEC_OFFSET(vector, type, iter) \
	((pos_t)(iter - (type)vec_idx2addr(vector, 0)))

#define VEC_FOREACH(vector, type, iter) \
	for (iter = (type)vec_idx2addr(vector, 0); \
		VEC_OFFSET(vector, type, iter) < vec_size(vector); \
		iter++)
#endif

