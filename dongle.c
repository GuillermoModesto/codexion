#include "codexion.h"

/* TODO: implement the real priority criterion.
 *
 * If sim->scheduler is "fifo": the smaller value = whoever has been waiting
 *   LONGER for this specific dongle. Hint: coder_t doesn't have a field yet
 *   for "since when am I waiting" - decide where to add it, and at what
 *   exact moment it gets filled in (review block 3: it must happen BEFORE
 *   the first cond_wait, exactly once).
 * If it's "edf": the smaller value = the closest deadline =
 *   compile_timer + time_to_burnout. Both fields already exist,
 *   in coder_t and sim_t.
 *
 * "ctx" will be your sim_t* (you decide that when calling heap_init). */
long	get_priority(void *ctx, int coder_id)
{
	/* TODO */
	(void)ctx;
	(void)coder_id;
	return (0);
}

/* TODO: the heart of the synchronization for ONE dongle. Reference
 * pseudocode (block 3):
 *
 *   lock(dongle->mutex)
 *   heap_push(&dongle->heap, coder->id)      // ONCE, before the first wait
 *   while (NOT (heap_peek_min(&dongle->heap) == coder->id
 *               AND dongle->estado == DONGLE_FREE))
 *       deadline = dongle->state_timer + duration_for(dongle->estado)
 *       pthread_cond_timedwait(&dongle->wait_room, &dongle->mutex, &deadline)
 *   heap_remove_id(&dongle->heap, coder->id)
 *   dongle->estado = DONGLE_IN_USE
 *   dongle->state_timer = now_ms()
 *   unlock(dongle->mutex)
 *
 * Hints:
 * - duration_for(state): COOLDOWN -> sim->dongle_cooldown,
 *   IN_USE -> sim->time_to_compile. What about FREE but not your turn
 *   (because the other coder also waiting on this dongle has higher
 *   priority)? There's no "physical" duration to pull from the dongle
 *   in that case - think about why, and what deadline would make sense.
 * - building the struct timespec from gettimeofday() was covered in block 1.
 */
static void	acquire_one_dongle(dongle_t *dongle, coder_t *coder, sim_t *sim)
{
	/* TODO */
	(void)dongle;
	(void)coder;
	(void)sim;
}

/* Already solved - this is the total-order rule you derived:
 * always request the LOWER-index dongle first, of your two possible ones. */
void	acquire_dongles(coder_t *coder, sim_t *sim)
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
	acquire_one_dongle(&sim->dongles[lower], coder, sim);
	acquire_one_dongle(&sim->dongles[higher], coder, sim);
}

/* TODO: the reverse of acquire_one_dongle - mark DONGLE_COOLDOWN,
 * update state_timer, broadcast. */
static void	release_one_dongle(dongle_t *dongle)
{
	/* TODO */
	(void)dongle;
}

/* Release order doesn't matter (reasoned through in block 3:
 * releasing can never form a cycle, only hold-while-waiting can). */
void	release_dongles(coder_t *coder, sim_t *sim)
{
	int	left;
	int	right;

	left = (coder->id - 1 + sim->number_of_coders) % sim->number_of_coders;
	right = coder->id;
	release_one_dongle(&sim->dongles[left]);
	release_one_dongle(&sim->dongles[right]);
}