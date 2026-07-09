/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_acquire.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 10:00:00 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 14:55:58 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

#define DONGLE_RETRY_MS 5

/* A dongle becomes usable either because it was never taken (FREE),
 * or because its cooldown has actually elapsed - nothing else in the
 * project ever flips COOLDOWN back to FREE explicitly, so "elapsed
 * cooldown" has to be checked here, on demand, by whoever is asking. */
static int	dongle_available(t_dongle *dongle, t_sim *sim)
{
	if (dongle->state == DONGLE_FREE)
		return (1);
	if (dongle->state == DONGLE_COOLDOWN)
		return (now_ms() - dongle->state_timer >= sim->dongle_cooldown);
	return (0);
}

/* Only ever called when we already know we can't proceed yet, so this
 * never has to consider "FREE and it's my turn".
 * - IN_USE / COOLDOWN-not-elapsed: a real physical deadline, anchored to
 *   when that state started.
 * - everything else (available, but someone else has priority - the
 *   "free but not your turn" case): there is no dongle-driven duration
 *   to anchor to, since nothing physical is happening on the dongle
 *   right now. We fall back to a short fixed poll instead of sleeping
 *   indefinitely, so the moment the other coder actually acts (and
 *   broadcasts), or even if it doesn't, we re-check soon either way. */
static long	next_deadline(t_dongle *dongle, t_sim *sim)
{
	if (dongle->state == DONGLE_IN_USE)
		return (dongle->state_timer + sim->time_to_compile);
	if (dongle->state == DONGLE_COOLDOWN
		&& now_ms() - dongle->state_timer < sim->dongle_cooldown)
		return (dongle->state_timer + sim->dongle_cooldown);
	return (now_ms() + DONGLE_RETRY_MS);
}

/* Two conditions to proceed (same skeleton as the wait/bell demo):
 * it is my turn in the queue AND the dongle is usable. The loop also
 * exits when the stop switch flips - otherwise a waiting coder would
 * sleep forever after the simulation ends and join would hang.
 * Returns 1 if acquired, 0 if the simulation stopped while waiting. */
static int	acquire_one_dongle(t_dongle *dongle, t_coder *coder, t_sim *sim)
{
	struct timespec	deadline;

	pthread_mutex_lock(&dongle->mutex);
	coder->wait_since = now_ms();
	heap_push(&dongle->heap, coder->id);
	while (!sim_should_stop(sim)
		&& !(heap_peek_min(&dongle->heap) == coder->id
			&& dongle_available(dongle, sim)))
	{
		ms_to_timespec(next_deadline(dongle, sim), &deadline);
		pthread_cond_timedwait(&dongle->wait_room, &dongle->mutex, &deadline);
	}
	heap_remove_id(&dongle->heap, coder->id);
	if (sim_should_stop(sim))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	dongle->state = DONGLE_IN_USE;
	dongle->state_timer = now_ms();
	pthread_cond_broadcast(&dongle->wait_room);
	pthread_mutex_unlock(&dongle->mutex);
	log_state(sim, coder->id, "has taken a dongle");
	return (1);
}

/* Already solved - this is the total-order rule you derived:
 * always request the LOWER-index dongle first, of your two possible ones.
 * Returns 0 (abort) if the simulation stopped mid-acquisition. */
int	acquire_dongles(t_coder *coder, t_sim *sim)
{
	int	left;
	int	right;
	int	lower;
	int	higher;

	left = (coder->id - 1 + sim->number_of_coders) % sim->number_of_coders;
	right = coder->id;
	if (left < right)
	{
		lower = left;
		higher = right;
	}
	else
	{
		lower = right;
		higher = left;
	}
	if (!acquire_one_dongle(&sim->dongles[lower], coder, sim))
		return (0);
	return (acquire_one_dongle(&sim->dongles[higher], coder, sim));
}
