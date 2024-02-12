#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

void printstr(char *str, int i)
{
	write(2, str, i);
}

void ft_error(char *str)
{
	int i = 0;

	while (str[i])
		i++;
	printstr(str, i);
}

int ft_exec(char **argv, char **env, int start, int i, int count_pipe)
{
	int fd[count_pipe][2];
	pid_t pid[count_pipe + 1];
	int status = 0;
	int index = 0;
	int j = start;

	if (strcmp(argv[start], ";") == 0)
	{
		return (0);
	}	
	while(index <= count_pipe)
	{
		if (count_pipe > 0 && index < count_pipe)
		{	
			pipe(fd[index]);
		}
		while(argv[j] && strcmp(argv[j], "|") != 0 && strcmp(argv[j], ";") != 0)
			j++;
		pid[index] = fork();
		if (pid[index] == 0)
		{
			if (count_pipe == 0)
				argv[i] = NULL;
			else
				argv[j] = NULL;
			if (index <= count_pipe && count_pipe != 0)
			{
				if (index == 0)
				{
					close(fd[index][0]);
					dup2(fd[index][1], 1);
					close(fd[index][1]);				
				}
				if (index > 0 && index < count_pipe)
				{
					close (fd[index][0]);
					dup2(fd[index][1], 1);
					close(fd[index][1]);
					dup2(fd[index - 1][0], 0);
					close (fd[index - 1][0]);
				}
				if (index > 0 && index == count_pipe)
				{

					dup2(fd[index - 1][0], 0);
					close(fd[index - 1][0]);
				}
			}
			execve(argv[start], argv + start, env);			
			ft_error("error: cannot execute ");
			ft_error(argv[start]);
			ft_error("\n");
			exit (1) ;
		}
		else
		{
			if (count_pipe > 0 && index <= count_pipe)
			{
				if(index == 0)
					close(fd[index][1]);
				else if (index < count_pipe)
				{
					close(fd[index][1]);
					close(fd[index - 1][0]);
				}
				else if (index == count_pipe)
				{
					close(fd[index - 1][0]);
				}
			}
			if (count_pipe == 0)
				break ;

		}
		index++;
		start = j + 1;
		j++;
	}			
	index = 0;
	while(index <= count_pipe)
	{
		waitpid(pid[index], &status, WUNTRACED);
		index++;
	}
	return(status);
}

void	ft_cd(char **argv, int i)
{
	if (argv[i + 1] == NULL || strcmp(argv[i + 1], ";") == 0)
	{
		ft_error("error: cd: bad arguments\n");
	}
	else if (chdir(argv[i + 1]) == -1)
	{
		ft_error("error: cd: cannot change directory to ");
		ft_error(argv[i + 1]);
		ft_error("\n");
	}
}

int main(int argc, char **argv, char **env)
{
	int start = 1;
	int i = 1;
	int count_pipe = 0;

	if (argc == 1)
		return (0);
	while (i < argc)
	{
		if (strcmp(argv[i], "cd") == 0)
		{
			ft_cd(argv, i);
			i++;
			start++;
			while (strcmp(argv[i], ";") == 0)
			{
				i++;
				start++;
			}
		}
		if (strcmp(argv[i], "|") == 0)
			count_pipe++;
		else if (i == argc - 1 || strcmp(argv[i], ";") == 0)
		{
			if (i == argc - 1 && strcmp(argv[i], ";") != 0)
				i = argc;
			ft_exec(argv, env, start, i, count_pipe);	
			count_pipe = 0;
			start = i + 1;
		}
		i++;
	}
	return (0);
}