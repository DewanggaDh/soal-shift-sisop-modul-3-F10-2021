#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/wait.h>
#include<unistd.h>

int writePipe (int *pipe) {
    close(pipe[0]);
    dup2(pipe[1], 1);
    close(pipe[1]);
    return 0;
}

int readPipe (int *pipe) {
    close(pipe[1]);
    dup2(pipe[0], 0);
    close(pipe[0]);
    return 0;
}

int main(){
    int p_1[2];
    int p_2[2];
    pid_t cpid1, cpid2, cpid3, status;
    if(pipe(p_1) == -1) {
        fprintf(stderr, "pipe failed");
        return 1;
    }
    if(pipe(p_2) == -1) {
        fprintf(stderr, "pipe failed");
        return 1;
    }

    cpid1 = fork();
    if (cpid1 < 0) fprintf(stderr, "fork failed");
    if (cpid1 == 0) {
        writePipe(p_1);

        char *argv1[] = {"ps", "aux", NULL};
        execv("/bin/ps", argv1);
    } 

    cpid2 = fork();
    if (cpid2 < 0) fprintf(stderr, "fork failed");
    if (cpid2 == 0) {
        readPipe(p_1);
        writePipe(p_2);

        char *argv1[] = {"sort", "-nrk", "3,3", NULL};
        execv("/bin/sort", argv1);
    }

     close(p_1[0]);
     close(p_1[1]);

    cpid3 = fork();
    if (cpid3 < 0) fprintf(stderr, "fork failed");
    if (cpid3 == 0) {
        readPipe(p_2);

        char *argv1[] = {"head", "-5", NULL};
        execv("/bin/head", argv1);
    }

     close(p_2[0]);
     close(p_2[1]);

    exit(EXIT_SUCCESS);
}