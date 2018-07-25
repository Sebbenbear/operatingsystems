#include <ctype.h> // character operations, like uppercase/lowercase/encoding
#include <errno.h> // test for error codes
#include <stdbool.h> // defines booleans so you don't need to use ints all the time
#include <stdio.h> // Core input/output functions
#include <stdlib.h> // memory allocation, process ctrl, random numbers, number conversions
#include <string.h> // copy, concat, tokenize and other string operations found here
#include <sys/types.h> // gives you nice data types for managing OS concepts
#include <signal.h> // provides methods for dealing with signals (the handlers for interrupts)
#include <sys/wait.h> // 
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include "tokenizer.h"
#include <sys/stat.h>
#include <limits.h>

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

// TODO: MAKE A DISCO SHELL - NOT FOR PEOPLE SENSITIVE TO FLASHING LIGHTS
#define RED   "\x1B[31m" // define the color for the terminal
#define RESET "\x1B[0m" // reset back to the normal color
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

/* Define the functions in the program*/
int cmd_exit(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
int cmd_pwd(struct tokens *tokens);
int cmd_cd(struct tokens *tokens);
int execute_program(struct tokens *tokens);
int file_exists(char *filename);

/* Helper functions */
bool is_valid_directory(char *directory_path);

/* Built-in command functions take token array (see parse.h) and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_exit, "exit", "exit the command shell"},
  {cmd_pwd, "pwd", "print the working directory"},
  {cmd_cd, "cd", "change the working directory"},
};

bool is_valid_directory(char *directory_path) {
  DIR *dir = opendir(directory_path);
  if (dir) {
    closedir(dir);
    return true;
  } else {
    return false;
  }
}

int file_exists(char *filename)
{
  struct stat buffer;
  return (stat (filename, &buffer) == 0);
}

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) {
  exit(0);
}

/* Prints the current working directory 
  // From unistd.h, pass in a buffer and the size of the buffer
  // char *getcwd(char *buf, size_t size);
*/
int cmd_pwd(unused struct tokens *tokens) { // if the token isn't used, compiler won't complain
  char cwd[1024]; // create a buffer up to 1024 bytes
  printf(RED "%s\n" RESET, getcwd(cwd, sizeof(cwd))); // pass in the command, which uses the buffer and the size of the buffer and stores the result in the buffer
  return 1;
}

int cmd_cd(struct tokens *tokens) {
  // // check the number of arguments is correct
  int number_of_tokens = tokens_get_length(tokens);
  if (number_of_tokens != 2) {
    printf("%s\n", "You need to specify which directory to cd into.");
    return 1;
  }

  char *directory_path = tokens_get_token(tokens, 1);

  if (is_valid_directory(directory_path)) {
    printf("Changing to %s\n", directory_path);
    chdir(directory_path);
  } else {
    printf("%s\n", "That directory doesn't exist.");
  }
  return 1;
}

void get_arguments(struct tokens *tokens, char **arguments) {
  int number_of_tokens = tokens_get_length(tokens); 

  int i;
  for (i = 0; i < number_of_tokens; i++) {
    arguments[i] = tokens_get_token(tokens, i);
  }
  arguments[number_of_tokens] = NULL;
}

//     /usr/bin/wc shell.c
//     /usr/bin/
int execute_program(struct tokens *tokens) {

  if (tokens_get_length(tokens) < 1) {
    return 1;
  }
  
  char *directory_path = tokens_get_token(tokens, 0);

  // can't use this - only considers file paths that are directories
  // if (!is_valid_directory(directory_path)) {
  //   printf("%s\n", "That path is not valid.");
  //   return 1;
  // }

  // get the directory of the cv
  char concatPath[PATH_MAX];
  if (*directory_path == '.') { // dereference to get the first path
    char* path = getenv("PATH");
    char* token;
    
    while ((token = strsep(&path, ":"))) {
      strcpy(concatPath, token);
      strcat(concatPath, directory_path + 1);
      if (file_exists(concatPath)) {
        // we found it!
        printf("%s\n", concatPath);
        break;
      }
    } 
  } else if (*directory_path == '/' && file_exists(directory_path)) {
    printf("we're good to go!");
    strcpy(concatPath, directory_path);
  }

  // if (file_exists(directory_path)) {
  //   printf("exists!");
  // } else {
  //   printf("doesn't exist! checking path now...");
    //bool file_found = false;
    
    // do {
    //   // if there's no more paths, exit early

    //   // otherwise get the next path to check it
        

    // } while (!file_found);
  //}




  
  // printf("%s\n", "getting pid");

  pid_t cpid, status; // set up the pids for the child and parent processes
  
  //pid_t cpid, mypid, tcpid;
  //mypid = getpid();

  cpid = fork();
  if (cpid > 0) {         // if it's not the parent process
      // wait(&status);				/* Wait for child process to end */
      wait(&status);
      // printf("Child is done executing %d\n", tcpid);
  } else if (cpid == 0) { // if it's a child process

      // if you knew how many arguments in advance you had, use this
      // int result = execl(directory_path, tokens_get_token(tokens, 1), (char *) NULL);

      char *arguments[tokens_get_length(tokens) + 1]; // Plus 1 for NULL space, containers pointers to other strings
      get_arguments(tokens, arguments);

      int result = execv(concatPath, arguments);
      if (result == -1) {
        printf("Program did not execute correctly\n");
      }
      // printf("[%d] child\n", mypid); // print the current pid, and the childs pid
  } else {
      perror("Fork failed\n"); // when would this fail?
      exit(1);
  }
  // printf("[%d] DONE\n", mypid); // print the current pid
  return 1;

}

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive) {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

int main(unused int argc, unused char *argv[]) {
  init_shell();

  static char line[4096];
  int line_num = 0;

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin)) {
    /* Split our line into words. */
    struct tokens *tokens = tokenize(line);

    /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));

    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } else {
      /* REPLACE this to run commands as programs. */
      // fprintf(stdout, "This shell doesn't know how to run programs.\n");
      execute_program(tokens);
    }

    if (shell_is_interactive)
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);

    /* Clean up memory */
    tokens_destroy(tokens);
  }

  return 0;
}
