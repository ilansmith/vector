#include <stdlib.h>
#include <stdio.h>
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

static void element_add_print(vector_t vec, int idx, int val)
{
	printf("vector[%d] = %d, size:%d\n", idx, val, vec_size(vec));
}

static void do_test(void)
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
}

int main(int argc, char **argv)
{
	do_test();
	return 0;
}

