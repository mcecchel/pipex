/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcecchel <mcecchel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 17:53:49 by mcecchel          #+#    #+#             */
/*   Updated: 2025/04/02 14:32:43 by mcecchel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void open_files(char *infile, char *outfile, int *fd_in, int *fd_out)
{
	*fd_in = open(infile, O_RDONLY);
	if (*fd_in == -1)
	{
		perror("Error: Failed to open input file\n");
		exit(1);
	}
	// Apre il file di output in scrittura (crea se non esiste, sovrascrive se esiste)
	*fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);// oppure 0644
	if (*fd_out == -1)
	{
		perror("Error: Failed to open output file\n");
		close(*fd_in);
		exit(1);
	}
}

void	create_pipe(int *fd_pipe)
{
	if (pipe(fd_pipe) == -1)
	{
		perror("Error: Failed to create pipe\n");
		exit(1);
	}
}

char *get_command_path(char *cmd, char **envp)
{


}

void	create_first_process(int *fd_pipe, char *cmd1, char **envp)
{
	int pid;

	pid = fork();
	if (pid == -1)
	{
		perror("Error: Failed to create process\n");
		exit(1);
	}
	if (pid == 0)// Siamo nel primo child
	{
		close(fd_pipe[0]);// Chiudo la lettura
		// Reindirizzo stdout alla pipe
		if (dup2(fd_pipe[1], STDOUT_FILENO) == -1)
		{
			perror("Error: Failed to redirect stdout\n");
			exit(1);
		}
		close(fd_pipe[1]);// Chiudo la scrittura

		// esecuzione del comando

	}
}

void	create_second_process(int *fd_pipe)
{
	int pid;

	pid = fork();
	if (pid == -1)
	{
		perror("Error: Failed to create process\n");
		exit(1);
	}
	if (pid == 0)// Secondo child
	{
		close(fd_pipe[1]);// Chiudo la scrittura
		// Reindirizzo stdin alla pipe
		if (dup2(fd_pipe[0], STDIN_FILENO) == -1)
		{
			perror("Error: Failed to redirect stdin\n");
			exit(1);
		}
		close(fd_pipe[0]);// Chiudo la lettura

		// esecuzione del comando
	}
}

int main(int ac, char **av, char **envp)
{
	int fd_in;
	int fd_out;
	int fd_pipe[2];// Dove fd[0] = lettura, fd[1] = scrittura

	// Gestione numero argomenti
	if (ac != 5)
	{
		perror("Error: Wrong number of arguments\n");
		exit(1);
	}
	// Gestione apertura file
	open_files(av[1], av[4], &fd_in, &fd_out);
	ft_printf("Files opened correctly\n");
	// Gestione pipe
	create_pipe(fd_pipe);
	ft_printf("Pipe created correctly\n");
	// Gestione esecuzione comandi

	close(fd_in);
	close(fd_out);
	wait(NULL);
	wait(NULL);
	return (0);
}