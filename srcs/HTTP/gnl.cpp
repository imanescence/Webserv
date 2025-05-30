
#include "gnl.hpp"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	if (!str)
		return (0);
	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_bzero(void *s, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		*((unsigned char *)s + i) = 0;
		i++;
	}
}

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;

	if (!dest || !src)
		return (0);
	if (size == 0)
		return (ft_strlen(src));
	i = 0;
	while (src[i] && i < size - 1)
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	while (src[i])
		i++;
	return (i);
}

static char	*ft_addbuffer(char *str, char *buffer)
{
	char	*newstr;
	int		i;
	int		len;

	if (!str && !buffer[0])
		return (0);
	len = ft_strlen(str);
	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;
	newstr = (char*)malloc(sizeof(*newstr) * (len + i + 1));
	if (!newstr)
	{
		free(str);
		return (0);
	}
	ft_strlcpy(newstr, str, len + 1);
	newstr[len + i] = 0;
	while (i--)
		newstr[len + i] = buffer[i];
	free(str);
	return (newstr);
}

static void	ft_cleanbuffer(char *buffer)
{
	int		i;
	int		j;

	i = 0;
	while (buffer[i] && buffer[i] != '\n')
		i++;
	if (buffer[i] == '\n')
		i++;
	j = 0;
	while (i + j < BUFFER_SIZE)
	{
		buffer[j] = buffer[i + j];
		j++;
	}
	ft_bzero(&buffer[j], BUFFER_SIZE + 1 - j);
}

static int	ft_read_file(char *buffer, int fd)
{
	int	nbr_bytes;

	nbr_bytes = ft_strlen(buffer);
	if (nbr_bytes > 0)
		return (nbr_bytes);
	nbr_bytes = read(fd, buffer, BUFFER_SIZE);
	if (nbr_bytes == -1)
		return (-1);
	buffer[nbr_bytes] = 0;
	return (nbr_bytes);
}

char	*ft_error_read(char *next_line, char *buffer)
{
	ft_bzero(buffer, BUFFER_SIZE + 1);
	free(next_line);
	return (NULL);
}

char	*get_next_line(int fd)
{
	char		*next_line;
	static char	buffer[FOPEN_MAX][BUFFER_SIZE + 1];
	int			bytes_read;

	if (read(fd, 0, 0) < 0)
	{
		if (fd >= 0)
			ft_bzero(buffer[fd], BUFFER_SIZE + 1);
		return (0);
	}
	next_line = NULL;
	bytes_read = ft_read_file(buffer[fd], fd);
	while (bytes_read > 0)
	{
		next_line = ft_addbuffer(next_line, buffer[fd]);
		ft_cleanbuffer(buffer[fd]);
		if (!next_line || !next_line[0])
			return (0);
		if (next_line[ft_strlen(next_line) - 1] == '\n')
			return (next_line);
		bytes_read = ft_read_file(buffer[fd], fd);
	}
	if (bytes_read == -1)
		next_line = ft_error_read(next_line, buffer[fd]);
	return (next_line);
}