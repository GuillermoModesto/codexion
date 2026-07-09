/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:59:05 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:59:07 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* This is the ONE place the scheduler decision is made: hand the
 * heap the right priority function once, and nothing else in the
 * whole program ever branches on sim->scheduler again. */
static int	init_one_dongle(t_sim *sim, t_dongle *dongle)
{
	t_priority_fn	prio;

	prio = edf_priority;
	if (strcmp(sim->scheduler, "fifo") == 0)
		prio = fifo_priority;
	if (heap_init(&dongle->heap, 2, sim, prio) != 0)
		return (-1);
	dongle->state = DONGLE_FREE;
	dongle->state_timer = now_ms();
	pthread_mutex_init(&dongle->mutex, NULL);
	pthread_cond_init(&dongle->wait_room, NULL);
	return (0);
}

/* compile_timer starts at sim_start: the subject says burnout is
 * measured "from the start of the simulation" until first compile. */
static void	init_one_coder(t_sim *sim, t_coder *coder, int id)
{
	coder->id = id;
	coder->compile_amount = 0;
	coder->compile_timer = sim->sim_start;
	coder->wait_since = 0;
	coder->general_ref = sim;
	pthread_mutex_init(&coder->state_mutex, NULL);
}

/* Mirror of cleanup_sim for a partially built sim: only the first
 * "created" dongle/coder pairs exist yet, so only those (plus the
 * global mutexes and the arrays) are torn down. free(NULL) is ok. */
static void	undo_init(t_sim *sim, int created)
{
	int	i;

	i = 0;
	while (i < created)
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

int	init_sim(t_sim *sim)
{
	int	i;

	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->stop_mutex, NULL);
	sim->stop = 0;
	sim->sim_start = now_ms();
	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->dongles || !sim->coders)
	{
		undo_init(sim, 0);
		return (-1);
	}
	i = 0;
	while (i < sim->number_of_coders)
	{
		if (init_one_dongle(sim, &sim->dongles[i]) != 0)
		{
			undo_init(sim, i);
			return (-1);
		}
		init_one_coder(sim, &sim->coders[i], i);
		i++;
	}
	return (0);
}
