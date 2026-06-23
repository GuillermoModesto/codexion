#include "heap.h"
#include <stdlib.h>

void	heap_init(t_heap *heap, int capacity, void *ctx, t_priority_fn priority)
{
	heap->ids = malloc(sizeof(int) * capacity);
	heap->size = 0;
	heap->capacity = capacity;
	heap->ctx = ctx;
	heap->priority = priority;
}

void	heap_destroy(t_heap *heap)
{
	free(heap->ids);
	heap->ids = NULL;
	heap->size = 0;
	heap->capacity = 0;
}