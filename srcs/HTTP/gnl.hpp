#ifndef GNL_HPP
# define GNL_HPP

# define BUFFER_SIZE 1

# include <stdlib.h>
# include <unistd.h>
# include <limits.h>
# include <stdio.h>

size_t	ft_strlen(const char *str);
void	ft_bzero(void *s, size_t n);
size_t	ft_strlcpy(char *dest, const char *src, size_t size);
char	*get_next_line(int fd);

#endif