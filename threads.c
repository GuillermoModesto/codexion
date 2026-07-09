/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:59:44 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:59:46 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	start_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			sim_set_stop(sim);
			wake_everyone(sim);
			while (--i >= 0)
				pthread_join(sim->coders[i].thread, NULL);
			return (-1);
		}
		i++;
	}
	return (0);
}

static void	join_coders(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

int	start_threads(t_sim *sim)
{
	if (start_coders(sim) != 0)
		return (-1);
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
	{
		sim_set_stop(sim);
		wake_everyone(sim);
		join_coders(sim);
		return (-1);
	}
	return (0);
}

void	join_threads(t_sim *sim)
{
	join_coders(sim);
	pthread_join(sim->monitor_thread, NULL);
}
