#include <stdlib.h>
#include <string.h>
#include "vector.h"

#define VEC_INIT_SIZE 4

struct vector {
	unsigned int last;
	unsigned int size;
	void **buf;
	vec_dtor dtor;
};

static inline unsigned int pos2size(unsigned int x)
{
	int i;

	for (i = 0; i < 7; i++)
		x |= x >> (1<<i);
	return ((x >> 1) + 1) << 2;
}

static inline void buf_realloc(struct vector *vec, unsigned int pos)
{
	void **buf;
	unsigned int size_required = pos2size(pos);
	
	if (size_required <= vec->size)
		return;

	buf = realloc(vec->buf, size_required * sizeof(void*));
	if (!buf)
		return;

	vec->buf = buf;
	memset(vec->buf + vec->size, 0, size_required - vec->size);
	vec->size = size_required;
}

static inline void do_replace(struct vector *vec, int pos, void *e)
{
	if (!vec->buf[pos])
		return;

	vec->dtor(vec->buf[pos]);
	vec->buf[pos] = e;
}

void vec_uninit(vector_t vector)
{
	struct vector *vec;

	if (!vector)
		return;

	vec = (struct vector*)vector;
	while (vec->size) {
		if (vec->buf[vec->size - 1])
			vec->dtor(vec->buf[vec->size - 1]);
		vec->size--;
	}

	free(vec->buf);
	free(vec);
}

vector_t vec_init(vec_dtor dtor)
{
	struct vector *vec;

	vec = malloc(sizeof(struct vector));
	if (!vec)
		return NULL;

	vec->buf = calloc(VEC_INIT_SIZE, sizeof(void*));
	if (!vec->buf) {
		vec_uninit(vec);
		return NULL;
	}

	vec->last = 0;
	vec->size = VEC_INIT_SIZE;
	vec->dtor = dtor ? dtor : free;

	return (vector_t)vec;
}

int vec_push_back(vector_t vector, void *e)
{
	struct vector *vec = (struct vector*)vector;

	if (vec->last == vec->size)
		return -1;

	vec->buf[vec->last++] = e;
	buf_realloc(vec, vec->last);
	return 0;
}

int vec_insert(vector_t vector, void *e, unsigned int pos)
{
	struct vector *vec = (struct vector*)vector;

	buf_realloc(vec, pos);
	if (vec->size <= pos)
		return -1;

	do_replace(vec, pos, e);

	if (vec->last < pos)
		vec->last = pos;

	return 0;
}

void vec_erase(vector_t vector, int pos)
{
	struct vector *vec = (struct vector*)vector;

	do_replace(vec, pos, NULL);
}

void *vec_at(vector_t vector, int pos)
{
	struct vector *vec = (struct vector*)vector;

	if (vec->size <= (unsigned int)pos)
		return NULL;

	return vec->buf[pos];
}

unsigned int vec_size(vector_t *vector)
{
	struct vector *vec = (struct vector*)vector;

	return vec->last;
}

unsigned int vec_for_each(vector_t vector, int (*func)(void *e, va_list va),
		...)
{
	struct vector *vec = (struct vector*)vector;
	va_list va;
	unsigned int i;

	va_start(va, func);
	for (i = 0; i < vec->size; i++) {
		if (!vec->buf[i])
			continue;

		if (func(vec->buf[i], va))
			break;
	}
	va_end(va);

	return i;
}

void **vec_idx2addr(vector_t vector, unsigned int idx)
{
	struct vector *vec = (struct vector*)vector;

	if (vec->last < idx)
		return NULL;

	return &vec->buf[idx];
}

