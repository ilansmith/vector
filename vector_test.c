#include <stdlib.h>
#include <stdio.h>
#include <unit_test.h>

#include "vector.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

struct element {
	int num;
};

static void element_del(void *e)
{
	struct element *el = (struct element*)e;

	printf("freeing element %d\n", el->num);
	free(el);
}

static struct element *element_init(int num)
{
	struct element *el;

	el = malloc(sizeof(struct element));
	if (!el)
		return NULL;

	el->num = num;
	return el;
}

#ifdef ELEMENT_PRINT
static void element_add_print(vector_t vec, int idx, int val)
{
	printf("vector[%d] = %d, size:%d\n", idx, val, vec_size(vec));
}
#else
static void element_add_print(vector_t vec, int idx, int val)
{
}
#endif

static int test001(void)
{
	vector_t vec;
	int ret;

	vec = vec_init(NULL);
	if (!vec)
		return -1;

	ret = vec_size(vec) ? -1 : 0;
	vec_uninit(vec);

	return ret;
}

static int test002(void)
{
	vector_t vec;
	int ret = -1;
	int i;

	vec = vec_init(NULL);
	if (!vec) {
		printf("failed to initialize vector\n");
		return -1;
	}

	for (i = 0; i < 30; i++) {
		int capacity;
		int x = i;
		int j = 0;

		if (vec_push_back(vec, element_init(i))) {
			printf("failed to push back element\n");
			goto exit;
		}
		capacity = vec_capacity(vec);

		do {
			x >>= 1;
			j++;
		} while (x);
		j++;

		if (capacity != 1<<j) {
			printf("for element: %d, vector capacity: %d, "
				"expected: %d\n", i, capacity, 1<<j);
			goto exit;
		}
	}

	ret = 0;
exit:
	vec_uninit(vec);
	return ret;
}

static int test_100(void)
{
	vector_t vec;
	struct element **iter;
	int i, pos[] = { 20, 7, 0, 60, 13};

	printf("vector initialization\n");
	vec = vec_init(element_del);
	printf("vector size: %u\n", vec_size(vec));

	printf("\n");

	printf("pusing elements\n");
	for (i = 0; i < 15; i++) {
		if (vec_push_back(vec, element_init(i))) {
			printf("failed to push back element %d\n", i);
			continue;
		}
		element_add_print(vec, i, i);
	}

	printf("\n");

	printf("inserting lements\n");
	for (i = 0; i < ARRAY_SIZE(pos); i++) {
		int el_num = pos[i] + 30;
		vec_insert(vec, element_init(el_num), pos[i]);
		element_add_print(vec, pos[i], el_num);
	}

	printf("\n");

	printf("traversing vector (size:%d)\n", vec_size(vec));

	VEC_FOREACH(vec, iter) {
		if (!*iter) {
			printf("vec[%u] = NULL\n", VEC_OFFSET(vec, iter));
			continue;
		}

		printf("vec[%u] = %d\n", VEC_OFFSET(vec, iter), (*iter)->num);
	}

	printf("\n");

	printf("freeing vectors\n");
	vec_uninit(vec);

	return 0;
}

static struct single_test vector_tests[] = {
	{
		description: "Basic vector initialization",
		func: test001,
	},
	{
		description: "Vector capacity correctness",
		func: test002,
	},
	{
		description: "Simple test (unit test demo)",
		func: test_100,
		disabled:1
	},
};

static int vector_pre_test(void)
{
	return 0;
}

struct unit_test ut_vector = {
	.module = "vector",
	.description = "CPP std::Vector implementation in C",
	.pre_single_test = vector_pre_test,
	.tests = vector_tests,
	.count = ARRAY_SZ(vector_tests),
};

