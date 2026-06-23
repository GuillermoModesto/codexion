#include "heap.h"

static void	sift_down(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	smallest;
	int	done;

	done = 0;
	while (!done)
	{
		left = i * 2 + 1;
		right = i * 2 + 2;
		smallest = i;
		if (left < heap->size && heap->priority(heap->ctx, heap->ids[left])
			< heap->priority(heap->ctx, heap->ids[smallest]))
			smallest = left;
		if (right < heap->size && heap->priority(heap->ctx, heap->ids[right])
			< heap->priority(heap->ctx, heap->ids[smallest]))
			smallest = right;
		if (smallest == i)
			done = 1;
		else
		{
			heap_swap(heap, i, smallest);
			i = smallest;
		}
	}
}

void	heap_remove_id(t_heap *heap, int coder_id)
{
	int	i;
	int	pos;

	pos = -1;
	i = 0;
	while (i < heap->size && pos == -1)
	{
		if (heap->ids[i] == coder_id)
			pos = i;
		i++;
	}
	if (pos == -1)
		return ;
	heap->size--;
	heap->ids[pos] = heap->ids[heap->size];
	if (pos < heap->size)
	{
		sift_down(heap, pos);
		heap_sift_up(heap, pos);
	}
}