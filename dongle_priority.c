/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_priority.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:57:31 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:57:34 by guantino         ###   ########.fr       */
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
	return (sim->coders[coder_id].compile_timer);
}
