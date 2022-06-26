#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdlib.h>
# include <unistd.h>

#define BUFFER_SIZE 40

int		ft_strlen(const char *s);
int		ft_strlcpy(char *dst, const char *src, int dstsize);
char	*ft_substr(char const *s, int start, int len);
int		ft_strlcat(char *dst, const char *src, int dstsize);
char	*get_next_line(int fd);
char	*ft_strchr(const char *s, int c);
void	*ft_calloc(size_t count, size_t size);
void	*ft_memcpy(void *dst, const void *src, size_t n);
char	*ft_strdup(char **s1);

#endif
