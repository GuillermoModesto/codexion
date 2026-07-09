/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_priority.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 10:00:00 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 14:56:08 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	fifo_priority(void *ctx, int coder_id)
{
	t_sim	*sim;

	sim = (t_sim *)ctx;
	return (sim->coders[coder_id].wait_since);
}

long	edf_priority(void *ctx, int coder_id)
{
	t_sim	*sim;

	sim = (t_sim *)ctx;
	return (sim->coders[coder_id].compile_timer + sim->time_to_burnout);
}
