#include "codexion.h"

long	fifo_priority(void *ctx, int coder_id)
{
	sim_t	*sim;

	sim = (sim_t *)ctx;
	return (sim->coders[coder_id].wait_since);
}

long	edf_priority(void *ctx, int coder_id)
{
	sim_t	*sim;

	sim = (sim_t *)ctx;
	return (sim->coders[coder_id].compile_timer + sim->time_to_burnout);
}
