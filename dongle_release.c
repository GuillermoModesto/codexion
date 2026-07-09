/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_release.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:57:43 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:57:45 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	release_one_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->state = DONGLE_COOLDOWN;
	dongle->state_timer = now_ms();
	pthread_cond_broadcast(&dongle->wait_room);
	pthread_mutex_unlock(&dongle->mutex);
}

/* Release order doesn't matter (reasoned through in block 3:
 * releasing can never form a cycle, only hold-while-waiting can). */
void	release_dongles(t_coder *coder, t_sim *sim)
{
	int	left;
	int	right;

	left = (coder->id - 1 + sim->number_of_coders) % sim->number_of_coders;
	right = coder->id;
	release_one_dongle(&sim->dongles[left]);
	release_one_dongle(&sim->dongles[right]);
}
