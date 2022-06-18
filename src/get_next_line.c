/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrossma <tgrossma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/01 10:44:54 by lhoerger          #+#    #+#             */
/*   Updated: 2022/06/18 17:29:24 by tgrossma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../get_next_line.h"

/* 
This function joins s1 with s2.
@return: The function returns the joined string s_join.
*/

char	*ft_strjoin(char *s1, char *s2)
{
	char	*s_join;
	int		len_s1;
	int		len_s2;
	int		i;
	int		j;

	if (s1 == 0 || s2 == 0)
		return (0);
	len_s1 = ft_strlen(s1);
	len_s2 = ft_strlen(s2);
	s_join = (char *)malloc(sizeof(*s1) * (len_s1 + len_s2) + 1);
	if (s_join == NULL)
		return (NULL);
	i = -1;
	while (s1[++i] != '\0')
		s_join[i] = s1[i];
	j = 0;
	while (s2[j] != '\0')
		s_join[i++] = s2[j++];
	s_join[i] = '\0';
	free(s1);
	s1 = NULL;
	free(s2);
	s2 = NULL;
	return (s_join);
}

/* 
This function duplicates string s1.
@return: The function returns the duplicated s1 calles s2.
*/

char	*ft_strdup(char **s1)
{
	char	*s2;
	int		len;

	len = ft_strlen(*s1);
	s2 = (char *) ft_calloc(len + 1, sizeof(char));
	if (!s2)
		return (0);
	ft_memcpy((char *) s2, *s1, len + 1);
	free(*s1);
	*s1 = NULL;
	return (s2);
}

/* 
This function actually searches for the newline and saves the information read into line.
@return: The function returns a status:
	2	:	No newline found, continue searching
	1	:	Newline found, stop searching and return the line.
	0	:	EOF or Error
	to read or if sth went wrong, it returns NULL. 
*/

int	search_newline(char *buffer, int *j, char ***rest, char ***line)
{
	char	*chr;
	int		fndxst[2];
	char	*s;

	chr = NULL;
	fndxst[0] = BUFFER_SIZE;
	fndxst[1] = 2;
	if (*j == 0 || *j == -1)
		return (0);
	buffer[*j] = '\0';
	chr = ft_strchr(buffer, '\n');
	if (chr != NULL)
	{
		fndxst[1] = 1;
		fndxst[0] = chr - buffer + 1;
		*(*rest) = ft_substr(buffer, fndxst[0], BUFFER_SIZE - fndxst[1] + 1);
	}
	else if (*j < BUFFER_SIZE)
		fndxst[1] = 0;
	s = ft_substr(buffer, 0, fndxst[0]);
	if (*(*line))
		*(*line) = ft_strjoin(*(*line), s);
	else
		*(*line) = ft_strdup(&s);
	return (fndxst[1]);
}

/* 
This function checks if there is any rest from the last read, otherwise it reads until it finds a newline.
@return: The function returns a status, given by search_newline:
	2	:	No newline found, continue searching
	1	:	Newline found, stop searching and return the line.
	0	:	EOF or Error
	to read or if sth went wrong, it returns NULL. 
*/

int	ft_get_next_line_helper(char *buffer, int fd, char **rest, char **line)
{
	int	status;
	int	j;

	status = 2;
	j = BUFFER_SIZE;
	if (*rest != NULL && (*rest)[0] != 0)
	{
		 ft_memcpy(buffer, *rest, ft_strlen(*rest) + 1);
		 free(*rest);
		 *rest = NULL;
		 status = (search_newline(buffer, &j, &rest, &line));
	}
	while (status == 2 && j == BUFFER_SIZE)
	{
		j = read(fd, buffer, BUFFER_SIZE);
		status = search_newline(buffer, &j, &rest, &line);
		if (status != 2 && line == NULL)
		{
			free(buffer);
			buffer = NULL;
			return (0);
		}
	}
	return (status);
}

/* 
This function reads one line of the file of file descriptor fd.
@return: The function returns the line, which was read in from the fd. If there is no line anymore 
	to read or if sth went wrong, it returns NULL. 
*/

char	*get_next_line(int fd)
{
	char		*buffer;
	static char	*rest;
	char		*line;
	int			status;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	line = NULL;
	buffer = (char *) malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if ((!buffer))
		return (NULL);
	status = ft_get_next_line_helper(buffer, fd, &rest, &line);
	if (rest && status == 0)
		free(rest);
	free(buffer);
	buffer = NULL;
	return (line);
}
