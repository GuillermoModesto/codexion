/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:59:12 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:59:14 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* One key (log_mutex) protects stdout so two messages never mix
 * on the same line. Internally coders are 0..N-1 but the subject
 * wants 1..N, hence the +1. After the stop switch flips, regular
 * messages are silently dropped, so "burned out" is the last line. */
void	log_state(t_sim *sim, int coder_id, const char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&sim->log_mutex);
	if (!sim_should_stop(sim))
	{
		timestamp = now_ms() - sim->sim_start;
		printf("%ld %d %s\n", timestamp, coder_id + 1, msg);
	}
	pthread_mutex_unlock(&sim->log_mutex);
}

/* The burnout message is special: it flips the stop switch and
 * prints in the same log_mutex critical section, so nothing can
 * sneak a message in between "stop is set" and "burned out". */
void	log_burnout(t_sim *sim, int coder_id)
{
	long	timestamp;

	pthread_mutex_lock(&sim->log_mutex);
	sim_set_stop(sim);
	timestamp = now_ms() - sim->sim_start;
	printf("%ld %d burned out\n", timestamp, coder_id + 1);
	pthread_mutex_unlock(&sim->log_mutex);
}
