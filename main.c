/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 17:53:49 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/08 17:39:31 by mcecchel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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
	pipex.fd_in = open(av[1], O_RDONLY);
	if (pipex.fd_in == -1)
	{
		perror("Error: Failed to open input file\n");
		exit(1);
	}
	init_file(pipex, envp, av);
	// Creazione pipe
	create_pipe(pipex.fd_pipe);
	// Creazione primo processo
	create_first_process(pipex, envp);
	// Creazione secondo processo
	create_second_process(pipex, envp);
	// Chiudo i file descriptor
	close(pipex.fd_pipe[0]);
	close(pipex.fd_pipe[1]);
	close(pipex.fd_in);
	close(pipex.fd_out);
	// Aspetto che i processi terminino
	waitpid(pipex.child_1, NULL, 0);
	waitpid(pipex.child_2, NULL, 0);
	// Libero la memoria
	clean_all(pipex);
	return (0);
}
