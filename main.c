/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 17:53:49 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/18 15:07:46 by mcecchel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int	main(int ac, char **av, char **envp)
{
	t_pipex	pipex;

	pipex = (t_pipex){0};
	// Gestione numero argomenti
	if (ac != 5)
	{
		perror("Error: Wrong number of arguments\n");
		exit(1);
	}
	if (pipe(pipex.fd_pipe) == -1)
	{
		perror("Pipe failed");
		close(pipex.fd_pipe[0]);
		close(pipex.fd_pipe[1]);
		exit (1);
	}
	pipex.child_1 = fork();
	if (pipex.child_1 < 0)
	{
		perror("Fork failed");
		exit (1);
	}
	if (pipex.child_1 == 0) // Siamo nel primo child
		child_process(pipex, av, envp);
	pipex.child_2 = fork();
	if (pipex.child_2 < 0)
	{
		perror("Fork failed");
		exit (1);
	}
	if (pipex.child_2 == 0) // Siamo nel secondo child
		parent_process(pipex, av, envp);
	close(pipex.fd_pipe[0]);
	close(pipex.fd_pipe[1]);
	close(pipex.fd_in);
	close(pipex.fd_out);//se non va, sposta i close sotto waitpd
	int status;
	waitpid(pipex.child_1, NULL, 0);
	waitpid(pipex.child_2, &status, 0);
	clean_all(pipex);
	return (0);
}
