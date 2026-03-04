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
  char* executable_location;
};

struct command parse_buffer(char *buffer)
{
  char *first_word = strtok(buffer, " ");
  int first_word_len = (int)strlen(first_word);
  char *args = first_word + first_word_len + 1;

  if (first_word_len == 0)//Throw away if empty
    return (struct command){COMMAND_UNKNOWN};

  //Check for builtins
  if (strcmp(first_word, "echo") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_ECHO, args};
  if (strcmp(first_word, "exit") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_EXIT, args};
  if (strcmp(first_word, "type") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_TYPE, args};

  //Check for executables
  const char *env_path = getenv("PATH");
  char *path_copy = strdup(env_path);
  char *p = strtok(path_copy, ":");
  while (p) {
    char tmp[1024]; //build path to potential executable
    snprintf(tmp, sizeof(tmp), "%s/%s", p, first_word);
    if (access(tmp, X_OK) == 0)
    {
      char* exe_path = strdup(tmp);
      return (struct command){COMMAND_EXECUTABLE, BUILTIN_NONE, args, exe_path};
    }
    p = strtok(nullptr, ":");
  }

  //Nothing found
  return (struct command){COMMAND_UNKNOWN};;
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
    struct command c = parse_buffer(arguments);
    switch (c.command_type)
    {
    case COMMAND_BUILTIN:
      printf("%s is a shell builtin\n", arguments);
      break;
    case COMMAND_EXECUTABLE:
      printf("%s is %s\n", arguments, c.executable_location);
      break;
    default:
      printf("%s: not found\n", arguments);
    }
    break;
  default:
    printf("An error has occurred.\n");
    exit(1);
    break;
  }
}

void run_exec(char* executable_location, char* arguments)
{
  printf("%s\n", executable_location);
}

int main(int argc, char *argv[]) {
  char buffer[1024];
  while (1) //exit command terminates
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
      run_exec(c.executable_location, c.arguments);
      break;
    default:
    case COMMAND_UNKNOWN:
      printf("%s: command not found\n", buffer);
      break;
    }
  }
}

