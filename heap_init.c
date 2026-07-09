/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:58:12 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:58:15 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"
#include <stdlib.h>

int	heap_init(t_heap *heap, int capacity, void *ctx, t_priority_fn prio)
{
	heap->ids = malloc(sizeof(int) * capacity);
	if (!heap->ids)
		return (-1);
	heap->size = 0;
	heap->capacity = capacity;
	heap->ctx = ctx;
	heap->priority = prio;
	return (0);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->ids);
	heap->ids = NULL;
	heap->size = 0;
	heap->capacity = 0;
}
