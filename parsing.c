#include "codexion.h"
#include <limits.h>

static int	ft_str_to_long(const char *s, long *out)
{
	long	result;
	int		i;

	if (!s || s[0] == '\0')
		return (-1);
	result = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		if (result > (LONG_MAX - (s[i] - '0')) / 10)
			return (-1);
		result = result * 10 + (s[i] - '0');
		i++;
	}
	*out = result;
	return (0);
}

static int	ft_is_valid_scheduler(const char *s)
{
	return (strcmp(s, "fifo") == 0 || strcmp(s, "edf") == 0);
}

static char *ft_strncpy(char *dest, const char *src, size_t n)
{
    size_t i;

    while (i < n && src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    while (i < n)
    {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

int	parse_args(sim_t *sim, char **argv)
{
	long	values[7];
	int		i;

	i = 0;
	while (i < 7)
	{
		if (ft_str_to_long(argv[i + 1], &values[i]) != 0)
			return (-1);
		i++;
	}
	if (!ft_is_valid_scheduler(argv[8]))
		return (-1);
	/* TODO: decide si number_of_coders == 0 (values[0]) es valido o no,
	 * el enunciado no lo dice explicitamente. */
	sim->number_of_coders = (int)values[0];
	sim->time_to_burnout = values[1];
	sim->time_to_compile = values[2];
	sim->time_to_debug = values[3];
	sim->time_to_refactor = values[4];
	sim->number_of_compiles_required = (int)values[5];
	sim->dongle_cooldown = values[6];
	ft_strncpy(sim->scheduler, argv[8], 4);
	sim->scheduler[4] = '\0';
	return (0);
}