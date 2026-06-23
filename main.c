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
	sim_t	sim;

	if (argc != 9)
	{
		print_usage(argv[0]);
		return (1);
	}
	if (parse_args(&sim, argv) != 0)
	{
		fprintf(stderr, "Error: invalid arguments\n");
		return (1);
	}
	/* TODO (block 3): init_dongles(&sim), init_coders(&sim)
	 * Inside init_dongles, for EACH dongle: call heap_init(&dongle->heap,
	 * 2, &sim, fifo_priority) if sim.scheduler is "fifo", or the same call
	 * with edf_priority otherwise. This is the ONE place that decision
	 * gets made - nothing past this point should ever branch on
	 * sim.scheduler again. */
	/* TODO (block 3): pthread_create for each coder + the monitor */
	/* TODO (block 3): pthread_join all of them */
	/* TODO (block 4/5): destroy mutexes/cond vars and free memory */
	return (0);
}