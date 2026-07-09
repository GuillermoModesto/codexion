/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:59:24 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:59:27 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_burned_out(t_coder *coder, t_sim *sim)
{
	long	last_start;

	pthread_mutex_lock(&coder->state_mutex);
	last_start = coder->compile_timer;
	pthread_mutex_unlock(&coder->state_mutex);
	return (now_ms() - last_start > sim->time_to_burnout);
}

static int	scan_for_burnout(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (coder_burned_out(&sim->coders[i], sim))
		{
			log_burnout(sim, i);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	all_compiled_enough(t_sim *sim)
{
	int	i;
	int	amount;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		amount = sim->coders[i].compile_amount;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (amount < sim->number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

/* Sleepers in timedwait might not check the stop switch for up to
 * their full deadline - ring every dongle's bell so they all wake,
 * re-check, see stop, and exit right away. */
void	wake_everyone(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].wait_room);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

/* The watcher (piece 1 again: just another thread). Scans every
 * ~1ms, well inside the 10ms burnout-print requirement. It is the
 * only thread that ever flips the stop switch. */
void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		running;

	sim = (t_sim *)arg;
	running = 1;
	while (running)
	{
		if (scan_for_burnout(sim))
			running = 0;
		else if (all_compiled_enough(sim))
		{
			sim_set_stop(sim);
			running = 0;
		}
		else
			usleep(1000);
	}
	wake_everyone(sim);
	return (NULL);
}
