#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum command
{
  unknown,
  echo,
  escape,
};

enum command parse_command(char *command)
{
  char *space = strchr(command, ' ');
  int command_len;
  if (space)
  {
    command_len = space-command;
  }
  else
  {
    command_len = (int)strlen(command);
  }

  if (strncmp(command, "echo", command_len) == 0)
    return echo;
  else if (strncmp(command, "exit", command_len) == 0)
    return escape;
  else
    return unknown;
}

int main(int argc, char *argv[]) {
  char command[1024];
  while (1)
  {
    setbuf(stdout, nullptr);
    printf("$ ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';

    switch (parse_command(command))
    {
    case echo:
      printf("%s\n", command+5);
      break;
    case escape:
      exit(0);
      break;
    case unknown:
      printf("%s: command not found\n", command);
      break;
    }
  }

  return 0;
}
