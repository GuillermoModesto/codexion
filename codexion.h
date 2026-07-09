/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guantino <guantino@student.42malaga.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 15:57:10 by guantino          #+#    #+#             */
/*   Updated: 2026/07/09 15:57:12 by guantino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include "heap.h"

typedef enum e_dongle_state
{
	DONGLE_FREE,
	DONGLE_IN_USE,
	DONGLE_COOLDOWN
}	t_dongle_state;

typedef struct s_sim	t_sim;

typedef struct s_dongle
{
	t_dongle_state		state;
	long				state_timer;
	pthread_mutex_t		mutex;
	pthread_cond_t		wait_room;
	t_heap				heap;
}	t_dongle;

typedef struct s_coder
{
	int					id;
	int					compile_amount;
	long				compile_timer;
	long				wait_since;
	pthread_t			thread;
	pthread_mutex_t		state_mutex;
	t_sim				*general_ref;
}	t_coder;

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

	t_dongle			*dongles;
	t_coder				*coders;

	pthread_mutex_t		log_mutex;
	long				sim_start;

	int					stop;
	pthread_mutex_t		stop_mutex;

	pthread_t			monitor_thread;
};

int		parse_args(t_sim *sim, char **argv);

long	now_ms(void);
void	ms_to_timespec(long ms, struct timespec *ts);

long	fifo_priority(void *ctx, int coder_id);
long	edf_priority(void *ctx, int coder_id);
int		acquire_dongles(t_coder *coder, t_sim *sim);
void	release_dongles(t_coder *coder, t_sim *sim);

int		sim_should_stop(t_sim *sim);
void	sim_set_stop(t_sim *sim);
void	sim_sleep(t_sim *sim, long duration_ms);

void	log_state(t_sim *sim, int coder_id, const char *msg);
void	log_burnout(t_sim *sim, int coder_id);

void	*coder_routine(void *arg);
void	*monitor_routine(void *arg);
void	wake_everyone(t_sim *sim);

int		init_sim(t_sim *sim);
void	cleanup_sim(t_sim *sim);
int		start_threads(t_sim *sim);
void	join_threads(t_sim *sim);

#endif
