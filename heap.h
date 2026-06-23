#ifndef HEAP_H
#define HEAP_H

typedef long	(*t_priority_fn)(void *ctx, int coder_id);

typedef struct s_heap
{
	int				*ids;
	int				size;
	int				capacity;
	void			*ctx;
	t_priority_fn	priority;
}	t_heap;

void	heap_init(t_heap *heap, int capacity, void *ctx, t_priority_fn priority);
void	heap_destroy(t_heap *heap);
void	heap_swap(t_heap *heap, int i, int j);
void	heap_sift_up(t_heap *heap, int i);
int		heap_peek_min(t_heap *heap);
int		heap_is_empty(t_heap *heap);
void	heap_push(t_heap *heap, int coder_id);
void	heap_remove_id(t_heap *heap, int coder_id);

#endif