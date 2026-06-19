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
	/* TODO (bloque 3): init_dongles(&sim), init_coders(&sim) */
	/* TODO (bloque 3): pthread_create de cada coder + del monitor */
	/* TODO (bloque 3): pthread_join de todos */
	/* TODO (bloque 4/5): destruir mutex/cond vars y liberar memoria */
	return (0);
}