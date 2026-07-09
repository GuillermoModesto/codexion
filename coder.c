/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 10:00:00 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 14:55:48 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* compile_timer = when my last compile STARTED (that is what both
 * burnout detection and edf deadlines are measured from). Written
 * under my state_mutex because the monitor reads it concurrently -
 * exact same race as the shared counter, same fix. */
static void	start_compiling(t_coder *coder, t_sim *sim)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->compile_timer = now_ms();
	pthread_mutex_unlock(&coder->state_mutex);
	log_state(sim, coder->id, "is compiling");
}

static void	finish_compiling(t_coder *coder, t_sim *sim)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->compile_amount++;
	pthread_mutex_unlock(&coder->state_mutex);
	release_dongles(coder, sim);
}

/* The life of one coder (piece 1: this is the function the thread
 * runs). Loop: grab both dongles -> compile -> give them back ->
 * debug -> refactor -> repeat, checking the stop switch between
 * phases so we exit promptly when the simulation ends. */
void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)arg;
	sim = coder->general_ref;
	while (!sim_should_stop(sim))
	{
		if (!acquire_dongles(coder, sim))
			break ;
		start_compiling(coder, sim);
		sim_sleep(sim, sim->time_to_compile);
		finish_compiling(coder, sim);
		if (sim_should_stop(sim))
			break ;
		log_state(sim, coder->id, "is debugging");
		sim_sleep(sim, sim->time_to_debug);
		if (sim_should_stop(sim))
			break ;
		log_state(sim, coder->id, "is refactoring");
		sim_sleep(sim, sim->time_to_refactor);
	}
	return (NULL);
}
