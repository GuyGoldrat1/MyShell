#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>



int handle_piping(int count, char** arglist, int pipe_pos){
    int pipefd[2]; // pipe array
    arglist[pipe_pos] = NULL;
    if (pipe(pipefd) == -1) {
        perror("Error - pipe problem");
        return 0;
    }
    
    pid_t pid_one = fork(); // creating first proces
    if (pid_one == -1) {
        perror("Error - Forking");
        return 0;
    } 
    // Child 1  process
    else if (pid_one == 0) {

        // ***********changing signal********
        if (signal(SIGINT, SIG_DFL) == SIG_ERR) { 
        perror("Error - changeing SIGINT handling");
        exit(1);
        }

        if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
            perror("Error - changeing SIGCHLD handling");
            exit(1);
        }
        // ***********changing signal********

        close(pipefd[0]);// Close read part

        if (dup2(pipefd[1], 1) == -1) {            //writing the standard output of this process to fd[1]
            perror("Error - stdout child ONE to the pipe");
            exit(1);
        }
        close(pipefd[1]); // Close fd    

        if (execvp(arglist[0], arglist) == -1) { // runing the process
            perror("Error - executing the command");
            exit(1);
        }
    }

    pid_t pid_two = fork(); // creating child process
    if (pid_two == -1) {
        perror("Error - Forking");
        return 0;
    } 
    // Child 2  process
    else if (pid_two == 0) {

        // ***********changing signal********
        if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("Error - changeing SIGINT handling");
        exit(1);
        }

        if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
            perror("Error - changeing SIGCHLD handling");
            exit(1);
        }
        // ***********changing signal********

        close(pipefd[1]);// Close write part

        if (dup2(pipefd[0], 0) == -1) { //   writing fd[0] to the input of the process
            perror("Error - stdout child TWO to the pipe");
            exit(1);
        }
        close(pipefd[0]); // Close fd

        if (execvp(arglist[pipe_pos + 1], arglist + pipe_pos + 1) == -1) { // runing the process
            perror("Error - executing the command");
            exit(1);
        }
    }
    close(pipefd[0]);
    close(pipefd[1]);

    if (waitpid(pid_one, NULL, 0) == -1 && errno != ECHILD && errno != EINTR){
        perror("Eror - waitpid");
        return 0;
    }
        if (waitpid(pid_two, NULL, 0) == -1 && errno != ECHILD && errno != EINTR){
        perror("Eror - waitpid");
        return 0;
    }

    return 1;
}

int handle_redirection(int count, char** arglist, int output) {
    arglist[count -2] = NULL;

    pid_t pid = fork();

    if (pid == -1) {
        perror("Error - Forking");
        return 0;
    } 

    // Child process
    else if (pid == 0) {

        if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
            perror("Error - changeing SIGINT handling");
            exit(1);
        }

        if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
            perror("Error - changeing SIGCHLD handling");
            exit(1);
        }

        if (output) {
            // Output redirection
            int fd = open(arglist[count - 1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if (fd == -1) {
                perror("Error - Opening output file");
                exit(1);
            }

            if (dup2(fd, 1) == -1) {
                perror("Error - stdout to the file");
                exit(1);
            }
            close(fd);

        } else {
            // Input redirection
            int fd = open(arglist[count - 1], O_RDONLY);
            if (fd == -1) {
                perror("Error - Opening input file");
                exit(1);
            }

            if (dup2(fd, 0) == -1) {
                perror("Error - stdin from the file");
                exit(1);
            }
            close(fd);
        }
        if (execvp(arglist[0], arglist) == -1) {
            perror("Error - executing the command");
            exit(1);
        }

    }

    if (waitpid(pid, NULL, 0) == -1 && errno != ECHILD && errno != EINTR){
        perror("Eror - waitpid");
        return 0;
    
    }
        return 1;
    
    }




int process_arglist(int count, char** arglist) {
    int pipe_pos = -1;
    int return_val = 1;
    int exe_background = 0; // tells the if to exeute code in the background
    int output = 0; // output = 0 - input redirect || output = 1 - output Redirect

    for (int i = 0; i < count; ++i) {
        if(*arglist[i] == '|'){
            pipe_pos = i;
        }
    }

    if(pipe_pos != -1){
        return_val = handle_piping(count, arglist,pipe_pos);
    }

    else if ((count > 1) && *arglist[count - 2] == '>'){
        int output = 1;
        return_val = handle_redirection(count, arglist, output);
    }
    else if ((count > 1) && *arglist[count - 2] == '<'){
        return_val = handle_redirection(count, arglist, output);
    }

    //Executing commands!! Background and eating
    else{

        if (*arglist[count - 1] == '&') {
            exe_background = 1;
            arglist[count - 1] = NULL;
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("Error - Forking");
            return 0;
        } 
        // Child process
        else if (pid == 0) {

            if(exe_background == 0){ 
                if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
                perror("Error - changeing SIGINT handling");
                exit(1);
            }
            }

            if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
                perror("Error - changeing SIGCHLD handling");
                exit(1);
            }

            if (execvp(arglist[0], arglist) == -1) {
                perror("Error - executing the command");
                exit(1);
            }
        }

        if(exe_background == 0){ // waits only if there is no &
            if (waitpid(pid, NULL, 0) == -1 && errno != ECHILD && errno != EINTR){
                perror("Eror - waitpid");
                return 0;
            
            }
        }
        
    }

    return return_val;
}

int prepare(void) {
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
        perror("Error - failed to change signal SIGINT handling");
        return -1;
    }
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) { 
        perror("Error - failed to change signal SIGCHLD handling");
        return -1;
    }
    return 0;

}

int finalize(void) {
    return 0;
}
