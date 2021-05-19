#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 

pthread_t tid[6]; //inisisasi banyaknya thread (dalam kasus ini 6 thread)
int arrays1[4][3], arrays2[3][6];
int size=1, size1=1;
int arrHasil[4][6];
int (* arrays3)[6];

void *multiplier(void *arg) {
    pthread_t id = pthread_self();

    for(int x=0; x<6; x++){
        if(pthread_equal(id,tid[x])){
            for (int i=0; i<4; i++) {
                arrHasil[i][x] = 0;
                for(int j=0; j<3; j++){
                    arrHasil[i][x] += arrays1[i][j]*arrays2[j][x];
                }
            }
        }
    }

}

void main()
{
    key_t key = 911;
    int shmid = shmget(key,sizeof(int[4][6]), IPC_CREAT | 0666); 
    arrays3 =  shmat(shmid,NULL,0);  
    int k=0, err;
    
    printf("Matriks 1:\n");
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<3 ;j++)
        {
            // arrays1[i][j] = size;
            // printf("%3d ", arrays1[i][j]);
            // size++;
            scanf("%d", &arrays1[i][j]);
        }
        printf("\n");
    }
    printf("\nMatriks2:\n");
    for(int i=0; i<3; i++)
    {
        for(int j=0; j<6 ;j++){
            // arrays2[i][j] = size1;
            // printf("%3d ", arrays2[i][j]);
            // size1++;
            scanf("%d", &arrays2[i][j]);
        }
        // printf("\n");
    }
    printf("\n");

    while(k<6)
    {
        err = pthread_create(&(tid[k]), NULL, &multiplier, NULL); //pembuatan thread
        if(err != 0){
            printf("Can't create thread : [%s]\n", strerror(err));
        }else{
            //printf("Crate thread success\n");
        }
        k++;
    }

    for (int i=0; i<6; i++) {
        pthread_join(tid[i], NULL);
    }
    
    printf("2a. Matriks hasil:\n");
    for(int i=0; i<4; i++){
        for(int k=0; k<6; k++)
        {
            arrays3[i][k] = arrHasil[i][k];
            printf("%d ", arrays3[i][k]);
        }
        printf("\n");
    }
    shmdt(arrays3);
    // shmctl(shmid, IPC_RMID, NULL);
    exit(0);
}