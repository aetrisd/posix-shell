#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
  BUILTIN_PWD,
  BUILTIN_NONE,
};

struct command
{
  enum command_type command_type;
  enum builtin builtin;
  int argument_count;
  char** arguments; //args_array[0] is the first word
  char* executable_location;
};

char** split_args(char* arguments, int *count)
{
  char *copy = strdup(arguments);
  char **result = nullptr;
  *count = 0;

  char *tok = strtok(copy, " ");
  while (tok) {
    char **tmp = realloc(result, sizeof(char *) * (*count + 1));
    if (!tmp) {
      for (int i = 0; i < *count; i++) free(result[i]);
      free(result);
      free(copy);
      *count = 0;
      return nullptr;
    }
    result = tmp;
    result[*count] = strdup(tok);
    (*count)++;
    tok = strtok(nullptr, " ");
  }

  free(copy);

  char **tmp = realloc(result, sizeof(char *) * (*count + 1));
  if (!tmp) {
    for (int i = 0; i < *count; i++)
      free(result[i]);
    free(result);
    *count = 0;
    return nullptr;
  }
  result = tmp;
  result[*count] = nullptr;

  return result;
}

struct command parse_buffer(char *buffer)
{
  if ((int)strlen(buffer) == 0)//Throw away if empty
    return (struct command){COMMAND_UNKNOWN};

  int argument_count = 0;
  char** arguments = split_args(buffer, &argument_count);

  //Check for builtins
  if (strcmp(arguments[0], "echo") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_ECHO, argument_count, arguments};
  if (strcmp(arguments[0], "exit") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_EXIT, argument_count, arguments};
  if (strcmp(arguments[0], "type") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_TYPE, argument_count, arguments};
  if (strcmp(arguments[0], "pwd") == 0)
    return (struct command){COMMAND_BUILTIN, BUILTIN_PWD, argument_count, arguments};

  //Check for executables
  const char *env_path = getenv("PATH");
  char *path_copy = strdup(env_path);
  char *p = strtok(path_copy, ":");
  while (p) {
    char tmp[1024]; //build path to potential executable
    snprintf(tmp, sizeof(tmp), "%s/%s", p, arguments[0]);
    if (access(tmp, X_OK) == 0)
    {
      char* exe_path = strdup(tmp);
      return (struct command){COMMAND_EXECUTABLE, BUILTIN_NONE, argument_count, arguments, exe_path};
    }
    p = strtok(nullptr, ":");
  }

  //Nothing found
  return (struct command){COMMAND_UNKNOWN};;
}

void run_builtin(enum builtin builtin, int argument_count, char** arguments)
{
  switch (builtin)
  {
  case BUILTIN_ECHO:
    for (int i = 1; i < argument_count; i++)
    {
      printf("%s ", arguments[i]);
    }
    printf("\n");
    break;
  case BUILTIN_EXIT:
    exit(0);
    break;
  case BUILTIN_TYPE:
    struct command c = parse_buffer(arguments[1]);
    switch (c.command_type)
    {
    case COMMAND_BUILTIN:
      printf("%s is a shell builtin\n", arguments[1]);
      break;
    case COMMAND_EXECUTABLE:
      printf("%s is %s\n", arguments[1], c.executable_location);
      break;
    default:
      printf("%s: not found\n", arguments[1]);
    }
    break;
  case BUILTIN_PWD:
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    break;
  default:
    printf("An error has occurred.\n");
    exit(1);
    break;
  }
}

void run_exec(char* executable_location, char** arguments)
{
  pid_t pid = fork();
  if (pid == 0) {
    execv(executable_location, arguments);
    exit(1);
  } else if (pid > 0) {
    wait(nullptr);
  }
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
      run_builtin(c.builtin, c.argument_count, c.arguments);
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

