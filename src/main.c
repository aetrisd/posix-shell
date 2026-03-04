#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum command_type
{
  COMMAND_BUILTIN,
  COMMAND_EXECUTABLE,
  COMMAND_UNKNOWN,
};

enum builtin
{
  BUILTIN_ECHO,
  BUILTIN_EXIT,
  BUILTIN_TYPE,
  BUILTIN_NONE,
};

struct command
{
  enum command_type command_type;
  enum builtin builtin;
  char* arguments;
};

struct command parse_buffer(char *buffer)
{
  char *first_word = strtok(buffer, " ");

  if (strlen(first_word) == 0)//Throw away if empty
    return (struct command){COMMAND_UNKNOWN, BUILTIN_NONE, ""};

  //Check for builtins
  if (strcmp(first_word, "echo") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_ECHO, buffer+5};
  if (strcmp(first_word, "exit") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_EXIT, buffer+5};
  if (strcmp(first_word, "type") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_TYPE, buffer+5};

  //Check for executables
  const char *path = getenv("PATH");
  char *p = strtok(path, ":");
  while (p) {
    //strcat(p, "/");
    //strcat(p, buffer);
    printf("%s\n", p);
    //if (access(p, X_OK) == 0)
      //return (struct command){COMMAND_EXECUTABLE, BUILTIN_NONE, buffer+command_len};
    p = strtok(nullptr, ":");
  }
  //printf("%s", path);

  return (struct command){COMMAND_UNKNOWN, BUILTIN_NONE, buffer};;
}

void run_builtin(enum builtin builtin, char* arguments)
{
  switch (builtin)
  {
  case BUILTIN_ECHO:
    printf("%s\n", arguments);
    break;
  case BUILTIN_EXIT:
    exit(0);
    break;
  case BUILTIN_TYPE:
    if (parse_buffer(arguments).command_type != COMMAND_UNKNOWN)
      printf("%s is a shell builtin\n", arguments);
    else
      printf("%s: not found\n", arguments);
    break;
  default:
    printf("An error has occurred.\n");
    exit(1);
    break;
  }
}

int main(int argc, char *argv[]) {
  char buffer[1024];
  while (1)
  {
    setbuf(stdout, nullptr);
    printf("$ ");
    fgets(buffer, sizeof(buffer), stdin);
    //Trim trailing newline. It trims the FIRST newline, not the last.
    buffer[strcspn(buffer, "\n")] = '\0';

    struct command c = parse_buffer(buffer);
    switch (c.command_type)
    {
    case COMMAND_BUILTIN:
      run_builtin(c.builtin, c.arguments);
      break;
    case COMMAND_EXECUTABLE:
      exit(0);
      break;
    default:
    case COMMAND_UNKNOWN:
      printf("%s: command not found\n", buffer);
      break;
    }
  }
}

