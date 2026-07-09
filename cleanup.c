/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 10:00:00 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 14:55:31 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Exact mirror of init_sim: every init'd mutex/cond/heap gets
 * destroyed, every malloc gets freed. Only called after all
 * threads have been joined, so nobody is using any of this. */
void	cleanup_sim(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		pthread_cond_destroy(&sim->dongles[i].wait_room);
		heap_destroy(&sim->dongles[i].heap);
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
		i++;
	}
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	free(sim->dongles);
	free(sim->coders);
}
