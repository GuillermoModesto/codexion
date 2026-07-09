/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_push.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 10:00:00 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 10:00:00 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

void	heap_sift_up(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (heap->priority(heap->ctx, heap->ids[i])
			>= heap->priority(heap->ctx, heap->ids[parent]))
			break ;
		heap_swap(heap, i, parent);
		i = parent;
	}
}

void	heap_push(t_heap *heap, int coder_id)
{
	if (heap->size >= heap->capacity)
		return ;
	heap->ids[heap->size] = coder_id;
	heap->size++;
	heap_sift_up(heap, heap->size - 1);
}
