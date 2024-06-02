# MyShell Project

## Overview

MyShell is a custom command-line shell implemented in C. It supports a variety of shell functionalities, including executing commands, handling pipes, redirecting input and output, and running commands in the background. The shell is designed to mimic some of the basic features found in Unix-like operating systems.

## Features

- **Command Execution:** Execute standard Unix commands.
- **Pipes:** Use the `|` symbol to pipe the output of one command as the input to another.
- **Redirection:** Redirect input and output using `<` and `>`.
- **Background Execution:** Run commands in the background using `&`.

## Files

### myshell.c

Contains the main logic for handling different shell functionalities, including command execution, piping, redirection, and background processes.

### shell.c

Contains the entry point of the shell, including initialization, command parsing, and cleanup.

## Usage

### Compilation

To compile the project, use the following command:

```bash
gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 shell.c myshell.c
```

### Running the Shell

After compiling, you can start the shell by running:

```bash
./myshell
```

### Commands

- **Standard Commands:** You can execute any standard Unix command.
  
  ```bash
  ls -l
  ```

- **Pipes:** Use the `|` symbol to pipe commands.

  ```bash
  ls -l | grep myfile
  ```

- **Redirection:** Redirect input and output using `<` and `>`.

  ```bash
  echo "Hello, World!" > output.txt
  cat < output.txt
  ```

- **Background Execution:** Run commands in the background using `&`.

  ```bash
  sleep 10 &
  ```

## Functions

### myshell.c

- **int handle_piping(int count, char** arglist, int pipe_pos):**
  Handles the creation and management of pipes between two commands.
  
- **int handle_redirection(int count, char** arglist, int output):**
  Handles input and output redirection for commands.

- **int process_arglist(int count, char** arglist):**
  Processes the command arguments to determine whether to handle piping, redirection, or execute commands in the foreground or background.

- **int prepare(void):**
  Initializes the shell, setting up necessary signal handlers.

- **int finalize(void):**
  Cleans up resources before exiting the shell.

### shell.c

- **int main(void):**
  Entry point of the shell. Handles initialization, command parsing, and cleanup.

## Signal Handling

- **SIGINT:** Ignored in the main shell but default handling is restored in child processes.
- **SIGCHLD:** Ignored in the main shell to prevent zombie processes, but default handling is restored in child processes.

## Error Handling

- Errors are handled using `perror` to provide detailed error messages for system call failures.
- Proper error handling is implemented for all major functions, including `fork`, `pipe`, `dup2`, `execvp`, and file operations.

