#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


pthread_t thd[3]; //inisialisasi array untuk menampung thread dalam kasus ini ada 2 thread
pid_t child;
int length=5;
int x=0;
char tanda[300][300]={};
int sinyal=0;

void* handler(void *arg)
{
    
    char things[200];
    strcpy(things,arg);

	unsigned long i=0;
	pthread_t id=pthread_self();
	int iter, index=0;
    char *arr2[50];
    char namaFile[200];
    char argCopy[100];

    //ngambil namafile beserta eksistensi
    char *token1 = strtok(things, "/");
    while (token1 != NULL) {
            arr2[index++] = token1;
            token1 = strtok(NULL, "/");
    }
    strcpy(namaFile,arr2[index-1]);

    //cek filenya termasuk dalam folder apa
    char *token = strchr(namaFile, '.');
    if (token == NULL) {
        strcat(argCopy, "Unknown");
    }
    else if (namaFile[0] == '.') {
        strcat(argCopy, "Hidden");
    }
    else {
        strcpy(argCopy, token+1);
        for (int i = 0; argCopy[i]; i++) {
            argCopy[i] = tolower(argCopy[i]);
        }
    }

    char source[1000], target[1000], dirToBeCreate[120];
    char *unhide, unhidden[200];
    strcpy(source, arg);
    if (sinyal == 1) {
        sprintf(target, "/home/allam/modul3/%s/%s", argCopy, namaFile);
        sprintf(dirToBeCreate, "/home/allam/modul3/%s/", argCopy);
        mkdir(dirToBeCreate, 0750);   
    } else if (sinyal == 2 || sinyal == 3) {
        if (namaFile[0] == '.') {
            namaFile[0] = '-';
        }
        sprintf(target, "%s/%s", argCopy, namaFile);
        sprintf(dirToBeCreate, "%s/", argCopy);
        mkdir(dirToBeCreate, 0750);
    }

    //pindah file
    if (rename(source,target) == 0) {
        printf("Berhasil Dikategorikan\n");
    } else printf("Sad,gagal :(\n");

    return NULL;
}

void listFilesRecursively(char *basePath)
{
	char path[256]={};
	struct dirent *dp;
	DIR *dir = opendir(basePath);

	if (!dir)
	return;

	while ((dp = readdir(dir)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			if (dp->d_type == DT_REG)
			{
				strcpy(tanda[x], basePath);
				strcat(tanda[x], dp->d_name);
				x++;
			}
			else
			{
				strcpy(path, basePath);
				strcat(path, dp->d_name);
				strcat(path, "/");
				listFilesRecursively(path);
			}
		}
	}
	closedir(dir);
}


int main(int argc, char *argv[]) {
    int i=0,j=0;

    if (strcmp(argv[1],"-f") == 0) {
        for(j = 2 ; j < argc ; j++ ){
            int err; sinyal = 1;
            printf("File %d : ", j-1);
            err = pthread_create(&(thd[i]),NULL,&handler,argv[j]);
            if (err != 0) {
                printf("error pthread create\n");
                // printf ("File %d : Sad,gagal :(\n", j-1);
            } //else printf("File %d : Berhasil Dikategorikan\n",j-1);
            
            pthread_join(thd[i++],NULL);
        }
    } else if (strcmp(argv[1],"-d") == 0) {
        i = 0; sinyal = 2;
	    int err;
	    listFilesRecursively(argv[2]);

	    for (i=0; i<x; i++){
		    err=pthread_create(&(thd[i]),NULL,&handler,(void *) tanda[i]);
		    if(err!=0)
		    {
			    printf("Yah, gagal disimpan :(\n");
		    }
	    }
	    
        for (i=0; i<x; i++){
		    pthread_join(thd[i],NULL);
        }

        printf("Direktori sukses disimpan!\n");
    }
    
    //mengkategorikan seluruh file yang ada di working directory
    else if (strcmp(argv[1],"*") == 0) {
        i = 0; sinyal = 3;
	   int err;
	    listFilesRecursively("/home/allam/Documents/GitHub/soal-shift-sisop-modul-3-F10-2021/soal3/");

	    for (i=0; i<x; i++){
		    err=pthread_create(&(thd[i]),NULL,&handler,(void *) tanda[i]);
		    
            if(err!=0){
			    return 0;
		    }
	    }

	    for (i=0; i<x; i++){
		    pthread_join(thd[i],NULL);
        }
    
    }
    return 0; 
}

//	./soal3 -f /home/allam/kategori1/caba.sh /home/allam/kategori1/iris.data /home/allam/kategori1/poto.png
//	./soal3 -d /home/allam/kategori1/
//	./soal3 \*