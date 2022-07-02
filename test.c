#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "vector.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define RUN_TEST(test) do { \
	printf("\n\n%s()\n", #test); \
	test(); \
} while (0)

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

	VEC_FOREACH(vec, struct element**, iter) {
		if (!*iter) {
			printf("vec[%u] = NULL\n", VEC_OFFSET(vec,
				struct element**, iter));
			continue;
		}

		printf("vec[%u] = %d\n", VEC_OFFSET(vec, struct element**,
			iter), (*iter)->num);
	}

	printf("\n");

	printf("freeing vectors\n");
	vec_uninit(vec);
}

static void push_sorted_vector(vector_t vec, ...)
{
	va_list ap;
	int num;
	int i = 0;

	printf("pushing sorted elements...\n");
	va_start(ap, vec);
	while ((num = va_arg(ap, int)) != -1) {
		printf("pushing element[%d]: %d\n", i++, num);
		vec_push_back(vec, element_init(num));
	}
	va_end(ap);
	printf("vector size: %u\n", vec_size(vec));
}

static void binary_search(vector_t vec, int search_val)
{
	struct element *e;
	int start = 0;
	int end = vec_size(vec) - 1;
	int mid;

	while (start <= end) {
		mid = (start + end) / 2;
		e = vec_at(vec, mid);

		if (e->num < search_val)
			start = mid + 1;
		else if (search_val < e->num)
			end = mid - 1;
		else
			break;
	}

	printf("number %2d: ", search_val);
	if (start <= end)
		printf("found at index %d\n", mid);
	else
		printf("not found\n");

}

static void do_binary_search(void)
{
	vector_t vec;

	printf("vector initialization\n");
	vec = vec_init(element_del);

	push_sorted_vector(vec, 2, 5, 8, 12, 16, 23, 38, 72, 91, -1);

	printf("\n");
	binary_search(vec, 23);
	binary_search(vec, 0);
	binary_search(vec, 91);
	binary_search(vec, 84);
	binary_search(vec, 13);
	binary_search(vec, 5);
	printf("\n");

	printf("freeing vectors\n");
	vec_uninit(vec);
}

int main(int argc, char **argv)
{
	RUN_TEST(do_test);
	RUN_TEST(do_binary_search);
	return 0;
}

