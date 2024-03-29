#include <stdlib.h>
#include <string.h>
#include "vector.h"

#define VEC_INIT_CAPACITY 4

struct vector {
	pos_t push_back_idx;
	pos_t capacity;
	void **buf;
	vec_dtor dtor;
};

/* Calculate the buffer capacity required for populating a given position, such
 * that the required capacity is double the closest power of 2 greater than
 * pos+1:
 *
 * capacity = first_pow2_greater_than(pos+1) * 2 = 32
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | | | | | | | | | | | |x| | | | | | | | | | | | | | | | | | | | |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  ^                     ^       ^                               ^
 *  0                    pos      15                              31
 *
 * Algorithm:
 *
 * 1. Paint 1 from the MSB to the LSB
 * 2. Add 1 for the closest power of 2 greater than the MSB
 * 3. Double this number
 */
static inline pos_t pos2capacity(pos_t x)
{
	uint64_t i;

	for (i = 0; i < 63; i++)
		x |= x >> ((uint64_t)1<<i);
	return ((x >> 1) + 1) << 2;
}

static inline int buf_realloc(struct vector *vec, pos_t pos)
{
	void **buf;
	pos_t capacity_required = pos2capacity(pos);
	
	if (capacity_required <= vec->capacity)
		return 0;

	buf = (void**)realloc(vec->buf, capacity_required * sizeof(void*));
	if (!buf)
		return -1;

	vec->buf = buf;
	memset(vec->buf + vec->capacity, 0,
		sizeof(void*) * (capacity_required - vec->capacity));
	vec->capacity = capacity_required;
	return 0;
}

static inline void do_replace(struct vector *vec, pos_t pos, void *e)
{
	if (vec->buf[pos] && vec->dtor)
		vec->dtor(vec->buf[pos]);

	vec->buf[pos] = e;
}

void vec_uninit(vector_t vector)
{
	struct vector *vec;

	if (!vector)
		return;

	vec = (struct vector*)vector;
	if (vec->dtor) {
		while (vec->capacity) {
			if (vec->buf[vec->capacity - 1])
				vec->dtor(vec->buf[vec->capacity - 1]);
			vec->capacity--;
		}
	}

	free(vec->buf);
	free(vec);
}

vector_t vec_init(vec_dtor dtor)
{
	struct vector *vec;

	vec = (struct vector*)malloc(sizeof(struct vector));
	if (!vec)
		return NULL;

	vec->buf = (void**)calloc(VEC_INIT_CAPACITY, sizeof(void*));
	if (!vec->buf) {
		vec_uninit(vec);
		return NULL;
	}

	vec->push_back_idx = 0;
	vec->capacity = VEC_INIT_CAPACITY;
	vec->dtor = dtor;

	return (vector_t)vec;
}

int vec_push_back(vector_t vector, void *e)
{
	struct vector *vec = (struct vector*)vector;

	if (!e)
		return -1;

	if (buf_realloc(vec, vec->push_back_idx))
		return -1;

	if (vec->push_back_idx == vec->capacity)
		return -1;

	vec->buf[vec->push_back_idx++] = e;
	return 0;
}

int vec_insert(vector_t vector, void *e, pos_t pos)
{
	struct vector *vec = (struct vector*)vector;

	if (!e)
		return -1;

	if (buf_realloc(vec, pos))
		return -1;

	if (vec->capacity <= pos)
		return -1;

	do_replace(vec, pos, e);

	if (vec->push_back_idx <= pos)
		vec->push_back_idx = pos + 1;

	return 0;
}

void vec_erase(vector_t vector, pos_t pos)
{
	struct vector *vec = (struct vector*)vector;

	do_replace(vec, pos, NULL);
}

void *vec_at(vector_t vector, pos_t pos)
{
	struct vector *vec = (struct vector*)vector;

	if (vec->capacity <= (pos_t)pos)
		return NULL;

	return vec->buf[pos];
}

pos_t vec_size(vector_t vector)
{
	struct vector *vec = (struct vector*)vector;

	return vec->push_back_idx;
}

pos_t vec_capacity(vector_t vector)
{
	struct vector *vec = (struct vector*)vector;

	return vec->capacity;
}

pos_t vec_for_each(vector_t vector, int (*func)(void *e, va_list va), ...)
{
	struct vector *vec = (struct vector*)vector;
	va_list va;
	pos_t i;

	va_start(va, func);
	for (i = 0; i < vec->capacity; i++) {
		if (!vec->buf[i])
			continue;

		if (func(vec->buf[i], va))
			break;
	}
	va_end(va);

	return i;
}

void **vec_idx2addr(vector_t vector, pos_t idx)
{
	struct vector *vec = (struct vector*)vector;

	if (vec->push_back_idx < idx)
		return NULL;

	return &vec->buf[idx];
}

