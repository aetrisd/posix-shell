#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum command
{
  unknown,
  echo,
  exit_command,
  type,
};

enum command parse_command(char *command)
{
  char *space = strchr(command, ' ');
  int command_len = space ? space-command : (int)strlen(command);
  if (command_len == 0)
    return unknown;

  if (strncmp(command, "echo", command_len) == 0)
    return echo;
  if (strncmp(command, "exit", command_len) == 0)
    return exit_command;
  if (strncmp(command, "type", command_len) == 0)
    return type;
  return unknown;
}

int main(int argc, char *argv[]) {
  char command[1024];
  while (1)
  {
    setbuf(stdout, nullptr);
    printf("$ ");
    fgets(command, sizeof(command), stdin);
    //Trim trailing newline. I don't exactly like this solution, probably gonna have to replace later if we need multi line support.
    command[strcspn(command, "\n")] = '\0';

    switch (parse_command(command))
    {
    case echo:
      printf("%s\n", command+5);
      break;
    case exit_command:
      exit(0);
      break;
    case type:
      if (parse_command(command+5) != unknown)
        printf("%s is a shell builtin\n", command+5);
      else
        printf("%s: not found\n", command+5);
      break;
    case unknown:
      printf("%s: command not found\n", command);
      break;
    }
  }
}

