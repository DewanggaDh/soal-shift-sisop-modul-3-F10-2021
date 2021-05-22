#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
pthread_t tid[24]; //inisisasi banyaknya thread (dalam kasus ini 24 thread)
pid_t child;
int controller = 0;
int const rows=4, cols=6;

int (* arrays3)[6],size=1, size1=1, counter=0;
long long arrHasil[4][6];
long long new[4][6];

void *multiplier(void *arg)  {
    pthread_t id = pthread_self();
    for (int row=0; row<rows; row++) {
        for (int col=0; col<cols; col++) {
            if (pthread_equal(id, tid[row*cols+col])) {
                if ((long long)arrays3[row][col] >= new[row][col]) {
                    long long batasAtas = arrays3[row][col], 
                              batasBawah = arrays3[row][col] - new[row][col];
                    arrHasil[row][col] = batasAtas--;
                    for (long long factorial = batasAtas; factorial>batasBawah; factorial--) {
                        arrHasil[row][col] = arrHasil[row][col] * factorial;
                    }
                } else if (arrays3[row][col] < new[row][col]) {
                    arrHasil[row][col] = (long long)arrays3[row][col];
                    for (long long factorial = (long long)arrays3[row][col]-1; factorial>0; factorial--) {
                        arrHasil[row][col] = arrHasil[row][col] * factorial;
                    }
                } 
                else if ((new[row][col]==0) || (arrays3[row][col]==0)) {
                    arrHasil[row][col] = 0;
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
    printf("2b. Matriks hasil:\n");
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
            scanf("%lld", &new[row][col]);
        }
    }

    while (k<cols*rows) {
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

    printf("\nMatriks factorial:\n");
    for (int row=0; row<rows; row++) {
        for (int col=0; col<cols; col++) {
            printf("%5lld ", arrHasil[row][col]);
        }
        printf("\n");
    }

    // shmdt(arrays3); 
    // shmctl(shmid,IPC_RMID,NULL); 
    exit(0);
}


// hasil perkalian 2a
// 13 12 16 12  8 18
//  8  9 10  7  6 11
// 12 14 20 10  4 18
//  7  9  7  6  8  9

// new arr
// 14  2  3  8  8 10
//  7  4  8  5 14  9
//  9  2 13  5 11  2
//  8  7 10  4 10  8