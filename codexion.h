#ifndef CODEXION_H
#define CODEXION_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

typedef enum e_dongle_state
{
	DONGLE_FREE,
	DONGLE_IN_USE,
	DONGLE_COOLDOWN
}	t_dongle_state;

typedef struct s_sim	sim_t;

typedef struct s_dongle
{
	t_dongle_state		state;
	long				state_timer;
	pthread_mutex_t		mutex;
	pthread_cond_t		wait_room;
}	dongle_t;

typedef struct s_coder
{
	int					id;
	int					compile_amount;
	long				compile_timer;
	pthread_t			thread;
	sim_t				*general_ref;
}	coder_t;

struct s_sim
{
	int					number_of_coders;
	long				time_to_burnout;
	long				time_to_compile;
	long				time_to_debug;
	long				time_to_refactor;
	int					number_of_compiles_required;
	long				dongle_cooldown;
	char				scheduler[5];

	dongle_t			*dongles;
	coder_t				*coders;

	pthread_mutex_t		log_mutex;
	long				sim_start;

	int					stop;
	pthread_mutex_t		stop_mutex;

	pthread_t			monitor_thread;
};

int		parse_args(sim_t *sim, char **argv);

#endif