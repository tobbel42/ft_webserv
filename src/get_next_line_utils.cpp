/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lhoerger <lhoerger@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/08 07:57:56 by lhoerger          #+#    #+#             */
/*   Updated: 2022/06/26 14:46:27 by lhoerger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../get_next_line.hpp"


/* 
This function allocates count * size memory and overwrites the memory with '\0'.
@return: The function returns a pointer pointing to the begin of the memory.
*/

void	*ft_calloc(size_t count, size_t size)
{
	void				*point;
	unsigned int		c;
	unsigned char		*t;

	c = 0;
	if (count == 0 || size == 0)
	{
		count = 1;
		size = 1;
	}
	point = (char *) malloc(size * count);
	if (!point)
		return ((void *) NULL);
	t = (unsigned char *)point;
	while (size * count > c)
	{
		t[c] = '\0';
		c++;
	}
	return ((void *) point);
}

/* 
This function copies n bytes from src to dst.
@return: The function returns dst.
*/

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*src2;
	unsigned char	*dest2;

	if (dst == 0 && src == 0)
		return (0);
	src2 = (unsigned char *) src;
	dest2 = (unsigned char *) dst;
	i = 0;
	while (i < n)
	{
		dest2[i] = src2[i];
		i++;
	}
	return (dst);
}

/* 
This function calculates the lenght of a string s.
@return: The function returns the lenght of string s.
*/

int	ft_strlen(const char *s)
{
	int	cnt;

	cnt = 0;
	if (!s)
		return (0);
	while (s[cnt] != '\0')
	{
		cnt++;
	}
	return (cnt);
}

/* 
This function creates a substring from s starting at start with the lenght of len.
@return: The function returns the substring.
*/

char	*ft_substr(char const *s, int start, int len)
{
	char	*s2;
	int		len_substr;

	if (!s)
		return (0);
	if (len == 0 || start >= ft_strlen(s))
	{
		return (NULL);
	}
	len_substr = ft_strlen(&s[start]);
	if (len_substr > len)
		len_substr = len;
	s2 = (char *) ft_calloc(len + 1, 1);
	if (!s2)
		return (0);
	ft_memcpy(s2, &s[start], len_substr);
	s2[len_substr] = '\0';
	return (s2);
}

/* 
This function searches fot c in the string s.
@return: The function returns a pointer to the location of c.
*/
char	*ft_strchr(const char *s, int c)
{
	int	cnt;
	int	len;

	len = ft_strlen(s);
	cnt = 0;
	while (cnt <= len)
	{
		if ((int) s[cnt] == c)
			return ((char *) &s[cnt]);
		cnt++;
	}
	return ((char *) 0);
}
