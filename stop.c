/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 10:00:00 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 10:00:00 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* The shared stop switch (piece 3: shared data + its own key).
 * Everyone reads it, the monitor writes it - so every access
 * grabs stop_mutex first. */
int	sim_should_stop(t_sim *sim)
{
	int	value;

	pthread_mutex_lock(&sim->stop_mutex);
	value = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (value);
}

void	sim_set_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}

/* Sleep in small chunks instead of one big usleep, checking the
 * stop switch each chunk - so a coder mid-debug notices the end
 * of the simulation within ~1ms instead of finishing its nap. */
void	sim_sleep(t_sim *sim, long duration_ms)
{
	long	end;

	end = now_ms() + duration_ms;
	while (!sim_should_stop(sim) && now_ms() < end)
		usleep(500);
}
