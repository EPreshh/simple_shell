#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


#define MAX_CMD_LEN 256
#define PROMPT "($) "

/**
 * execute_shell_command - Executes a given command using execve
 * @cmd: Pointer to the command to be executed
 * @env: Pointer to the environment variables
 *
 * This function creates a child process using fork.
 * The child process attempts to execute the command
 * passed in @cmd using execve and if the execve call
 * fails an error message is printed. The parent process
 * waits for the child process to terminate.
 */
void execute_shell_command(char *cmd, char **env)
{
	pid_t pid;
	int status;

	pid = fork();

	if (pid == 0)
	{
		char *argv[2];

		argv[0] = cmd;
		argv[1] = NULL;

		if (execve(cmd, argv, env) == -1)
		{
			perror("execve");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror("fork");
	} else
	{
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) &&
				!WIFSIGNALED(status));
	}
}

/**
 * main - Entry point for the simple shell program.
 * @argc: Count of parsed argument
 * @argv: Array of parsed argument
 * @envp: Pointer to environment variables
 *
 * This function implements a basic command-line shell.
 * In interactive mode, it displays a prompt and waits
 * for the user to enter a command. In non-interactive
 * mode, it reads commands from standard input.
 *
 * Return: Always (Success) 0.
 */
int main(int argc __attribute__((unused)),
		char *argv[] __attribute__((unused)), char *envp[])
{
	char cmd[MAX_CMD_LEN];
	char *newline;

	if (isatty(STDIN_FILENO))
	{
		while (1)
		{
			printf(PROMPT);

			if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL)
			{
				printf("\n");
				break;
			}
			newline = strchr(cmd, '\n');
			if (newline != NULL)
			{
				*newline = '\0';
			}
			if (strcmp(cmd, "exit") == 0)
				break;

			execute_shell_command(cmd, envp);
		}
	}
	else
	{
		while (fgets(cmd, MAX_CMD_LEN, stdin) != NULL)
		{
			newline = strchr(cmd, '\n');
			if (newline != NULL)
			{
				*newline = '\0';
			}

			execute_shell_command(cmd, envp);
		}
	}

	return (EXIT_SUCCESS);
}
