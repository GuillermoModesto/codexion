/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:59:17 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:59:20 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	print_usage(char *prog_name)
{
	fprintf(stderr,
		"Usage: %s number_of_coders time_to_burnout time_to_compile "
		"time_to_debug time_to_refactor number_of_compiles_required "
		"dongle_cooldown scheduler\n", prog_name);
}

int	main(int argc, char **argv)
{
	t_sim	sim;

	if (argc != 9)
	{
		print_usage(argv[0]);
		return (1);
	}
	if (parse_args(&sim, argv) != 0)
	{
		fprintf(stderr, "Error: %s\n", "invalid arguments");
		return (1);
	}
	if (init_sim(&sim) != 0)
		return (1);
	if (start_threads(&sim) != 0)
	{
		cleanup_sim(&sim);
		return (1);
	}
	join_threads(&sim);
	cleanup_sim(&sim);
	return (0);
}
