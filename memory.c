#include "shell.h"

/**
 * bfree - to free a ptr and repealed the address
 * @ptr: free ptr location
 *
 * Return: if free 1, if not 0.
 */
int bfree(void **ptr)
{
	if (ptr && *ptr)
	{
		free(*ptr);
		*ptr = NULL;
		return (1);
	}
	return (0);
}

