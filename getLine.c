#include "shell.h"

/**
 * input_buf - to buffer the commands connected
 * @info: para structure
 * @buf: buffer location
 * @len: len variable location
 *
 * Return: loaded bytes
 */
ssize_t input_buf(info_t *info, char **buf, size_t *len)
{
	ssize_t r = 0;
	size_t len_p = 0;

	if (!*len) /* if null buffer, fill */
	{
		/*bfree((void **)info->cmd_buf);*/
		free(*buf);
		*buf = NULL;
		signal(SIGINT, sigintHandler);
#if USE_GETLINE
		r = getline(buf, &len_p, stdin);
#else
		r = _getline(info, buf, &len_p);
#endif
		if (r > 0)
		{
			if ((*buf)[r - 1] == '\n')
			{
				(*buf)[r - 1] = '\0'; /* avoid dragging newline */
				r--;
			}
			info->linecount_flag = 1;
			remove_comments(*buf);
			build_history_list(info, *buf, info->histcount++);
			/* if (_strchr(*buf, ';')) is this a chain of command? */
			{
				*len = r;
				info->cmd_buf = buf;
			}
		}
	}
	return (r);
}

/**
 * get_input - acquires a line without the newline
 * @info: para structure
 *
 * Return: loaded bytes
 */
ssize_t get_input(info_t *info)
{
	static char *buf; /* the chain of command ';' buffer */
	static size_t i, j, len;
	ssize_t r = 0;
	char **buf_p = &(info->arg), *p;

	_putchar(BUF_FLUSH);
	r = input_buf(info, &buf, &len);
	if (r == -1) /* EOF */
		return (-1);
	if (len) /* we have leftover commands in the connected buffer */
	{
		j = i; /* initalize new iterator to current buffer position */
		p = buf + i; /* Acquire a return indicator */

		check_chain(info, buf, &j, i, len);
		while (j < len) /* repeat the semicolon or terminate */
		{
			if (is_chain(info, buf, &j))
				break;
			j++;
		}

		i = j + 1; /* passed augmentation ';'' */
		if (i >= len) /* is buffer termination reached? */
		{
			i = len = 0; /* reset length and position */
			info->cmd_buf_type = CMD_NORM;
		}

		*buf_p = p; /* give back pointer to the position of the current command */
		return (_strlen(p)); /* current command length of return */
	}

	*buf_p = buf; /* if not integrated, return from _getline() to buffer */
	return (r); /* length of return between the buffer and _getline() */
}

/**
 * read_buf - to load a buffer
 * @info: para structure
 * @buf: buffer
 * @i: size
 *
 * Return: r
 */
ssize_t read_buf(info_t *info, char *buf, size_t *i)
{
	ssize_t r = 0;

	if (*i)
		return (0);
	r = read(info->readfd, buf, READ_BUF_SIZE);
	if (r >= 0)
		*i = r;
	return (r);
}

/**
 * _getline - STDIN next input line
 * @info: para structure
 * @ptr: buffer ptr location, predetermined or NULL
 * @length: size of predetermined pointer buffer if not NULL
 *
 * Return: s
 */
int _getline(info_t *info, char **ptr, size_t *length)
{
	static char buf[READ_BUF_SIZE];
	static size_t i, len;
	size_t k;
	ssize_t r = 0, s = 0;
	char *p = NULL, *new_p = NULL, *c;

	p = *ptr;
	if (p && length)
		s = *length;
	if (i == len)
		i = len = 0;

	r = read_buf(info, buf, &len);
	if (r == -1 || (r == 0 && len == 0))
		return (-1);

	c = _strchr(buf + i, '\n');
	k = c ? 1 + (unsigned int)(c - buf) : len;
	new_p = _realloc(p, s, s ? s + k : k + 1);
	if (!new_p) /* MALLOC COLLAPSE! */
		return (p ? free(p), -1 : -1);

	if (s)
		_strncat(new_p, buf + i, k - i);
	else
		_strncpy(new_p, buf + i, k - i + 1);

	s += k - i;
	i = k;
	p = new_p;

	if (length)
		*length = s;
	*ptr = p;
	return (s);
}

/**
 * sigintHandler - to obstruct ctrl-C
 * @sig_num: the signal number
 *
 * Return: void
 */
void sigintHandler(__attribute__((unused))int sig_num)
{
	_puts("\n");
	_puts("$ ");
	_putchar(BUF_FLUSH);
}
