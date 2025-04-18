/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 18:37:33 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/18 16:17:18 by mcecchel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	find_spaces(char c)
{
	if (c == 32 || (c >= 9 && c <= 13))
		return (1);
	return (0);
}

// Estrae il valore della variabile d’ambiente PATH, che contiene i percorsi dove cercare i comandi
char	*find_env_path(t_pipex pipex)
{
	int	i;

	i = 0;
	while (pipex.envp[i])
	{
		if (ft_strncmp(pipex.envp[i], "PATH=", 5) == 0)
			return (pipex.envp[i] + 5);
		i++;
	}
	return (NULL);
}
/*
char	**split_cmd(const char *input)
{
	char	**args = malloc(100 * sizeof(char *));
	int		i = 0, j = 0, k = 0;
	int		in_quote = 0;
	char	quote_char = 0;
	char	buffer[1024];

	while (input[i])
	{
		if ((input[i] == '\'' || input[i] == '"') && !in_quote)
		{
			in_quote = 1;
			quote_char = input[i++];
			continue;
		}
		else if (in_quote && input[i] == quote_char)
		{
			in_quote = 0;
			i++;
			continue;
		}
		else if (input[i] == ' ' && !in_quote)
		{
			if (k > 0)
			{
				buffer[k] = '\0';
				args[j++] = strdup(buffer);
				k = 0;
			}
			i++;
			continue;
		}
		else
			buffer[k++] = input[i++];
	}
	if (k > 0)
	{
		buffer[k] = '\0';
		args[j++] = strdup(buffer);
	}
	args[j] = NULL;
	return (args);
} */

void	free_split(char **mat)
{
	int	i;

	if (!mat)
		return ;
	i = 0;
	while (mat[i])
	{
		free(mat[i]);// Libera ogni stringa
		i++;
	}
	free(mat);// Libera la matrice stessa
}

void	clean_all(t_pipex pipex)
{
	free_split(pipex.cmd.cmd1);
	free_split(pipex.cmd.cmd2);
	free(pipex.cmd.cmd1);
	free(pipex.cmd.cmd2);
	free(pipex.envp);
}
