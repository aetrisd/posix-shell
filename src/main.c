#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum builtin
{
  unknown,
  echo,
  exit_command,
  type,
};

enum builtin parse_buffer(char *buffer)
{
  const char *space = strchr(buffer, ' ');
  const int command_len = space ? space - buffer : (int)strlen(buffer);
  if (command_len == 0)
    return unknown;

  if (strncmp(buffer, "echo", command_len) == 0)
    return echo;
  if (strncmp(buffer, "exit", command_len) == 0)
    return exit_command;
  if (strncmp(buffer, "type", command_len) == 0)
    return type;
  return unknown;
}

int main(int argc, char *argv[]) {
  char buffer[1024];
  while (1)
  {
    setbuf(stdout, nullptr);
    printf("$ ");
    fgets(buffer, sizeof(buffer), stdin);
    //Trim trailing newline. I don't exactly like this solution, probably gonna have to replace later if we need multi line support.
    buffer[strcspn(buffer, "\n")] = '\0';


    switch (parse_buffer(buffer))
    {
    case echo:
      printf("%s\n", buffer+5);
      break;
    case exit_command:
      exit(0);
      break;
    case type:
      if (parse_buffer(buffer+5) != unknown)
        printf("%s is a shell builtin\n", buffer+5);
      else
        printf("%s: not found\n", buffer+5);
      break;
    case unknown:
      printf("%s: command not found\n", buffer);
      break;
    }
  }
}

