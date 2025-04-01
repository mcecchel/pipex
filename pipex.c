/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:16:21 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/01 19:00:26 by mcecchel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	open_files(t_pipex pipex, char **envp, char **av)
{
	pipex.envp = envp;
	pipex.fd_in = open(av[1], O_RDONLY);
	if (pipex.fd_in == -1)
	{
		perror("Error: Failed to open input file\n");
		exit(1);
	}
	// Apre il file di output in scrittura (crea se non esiste, sovrascrive se esiste)
	pipex.fd_out = open(av[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);// oppure 0644
	if (pipex.fd_out == -1)
	{
		perror("Error: Failed to open output file\n");
		close(pipex.fd_in);
		exit(1);
	}
// controllo da fare nel find path
	pipex.cmd.cmd1 = ft_split(av[2], ' ');
	pipex.cmd.cmd2 = ft_split(av[3], ' ');
	if (pipex.cmd.cmd1 == NULL || pipex.cmd.cmd2 == NULL)
	{
		perror("Error: Failed to split command\n");
		close(pipex.fd_in);
		close(pipex.fd_out);
		exit(1);
	}
}

char	*get_command_path(t_pipex pipex, char *cmd)
{
	int		i;
	char	*path_env;
	char	**path_split;
	char	*cmd_path;
	char	*temp;

	i = 0;
	path_env = find_env_path(pipex);
	if (!path_env || !(path_split = ft_split(path_env, ':')))
		return (NULL);
	while (path_split[i])
	{
		temp = ft_strjoin(path_split[i], "/");
		cmd_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(cmd_path, F_OK | X_OK) == 0)
		{
			ft_free_split(path_split);
			return (cmd_path);
		}
		free(cmd_path);
		i++;
	}
	ft_free_split(path_split);
	return (NULL);
}

int main(int ac, char **av, char **envp)
{
	t_pipex pipex;

	pipex = (t_pipex){0};
	// Gestione numero argomenti
	if (ac != 5)
	{
		perror("Error: Wrong number of arguments\n");
		exit(1);
	}
	// Gestione apertura file
	open_files(pipex, envp, av);
		ft_printf("Files opened correctly\n");
	// Gestione pipe
}