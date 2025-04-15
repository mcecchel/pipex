/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 18:37:33 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/15 18:31:14 by mcecchel         ###   ########.fr       */
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
