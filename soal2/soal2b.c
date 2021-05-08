#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
pthread_t tid[24]; //inisisasi banyaknya thread (dalam kasus ini 3 thread)
pid_t child;
int controller = 0;
int const rows=4, cols=6;

int (* arrays3)[6],size=1, size1=1, counter=0;
int arrHasil[4][6];
int new[4][6];

void *multiplier(void *arg)  {
    pthread_t id = pthread_self();
    for (int row=0; row<rows; row++) {
        for (int col=0; col<cols; col++) {
            counter++;
            if (pthread_equal(id, tid[counter])) {
                if (arrays3[row][col] < new[row][col]) {
                    new[row][col] = 0;
                } 
                else if ((new[row][col]==0) || (arrays3[row][col]==0)) {
                    arrHasil[row][col] = 0;
                }
                for (int factorial=arrays3[row][col]-1; factorial>new[row][col]; factorial--) {
                    arrHasil[row][col] *= factorial;
                }
            }
        }
    }
}

void main () {
    int k=0, err;
    key_t key = 911;

    int shmid = shmget(key,sizeof(int[rows][cols]), IPC_CREAT | 0666); 
    arrays3 =  shmat(shmid,NULL,0);
    printf("Matriks hasil:\n");
    for (int row=0; row<rows; row++) {
        for (int col=0; col<cols; col++) {
            // arrays3[row][col] = size++;
            printf("%3d ", arrays3[row][col]);
        }
        printf("\n");
    }

    for (int row=0; row<rows; row++) {
        for (int col=0; col<cols; col++) {
            arrHasil[row][col] = arrays3[row][col];
            scanf("%d", &new[row][col]);
        }
    }

    while (k<cols) {
        err = pthread_create(&(tid[k]), NULL, &multiplier, NULL);
        if(err != 0){
            printf("Can't create thread : [%s]\n", strerror(err));
        }else{
            //printf("Crate thread success\n");
        }
        k++;
    }

    for (int it=0; it<rows*cols; it++) {
        pthread_join(tid[it], NULL);
    }

    printf("\nMatriks Penjumlahan:\n");
    for (int row=0; row<rows; row++) {
        for (int col=0; col<cols; col++) {
            printf("%2d ", arrHasil[row][col]);
        }
        printf("\n");
    }

    // shmdt(arrays3); 
    shmctl(shmid,IPC_RMID,NULL); 
    exit(0);
}