/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:58:37 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:58:58 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

void	heap_swap(t_heap *heap, int i, int j)
{
	int	tmp;

	tmp = heap->ids[i];
	heap->ids[i] = heap->ids[j];
	heap->ids[j] = tmp;
}

int	heap_peek_min(t_heap *heap)
{
	if (heap->size == 0)
		return (-1);
	return (heap->ids[0]);
}

int	heap_is_empty(t_heap *heap)
{
	return (heap->size == 0);
}
