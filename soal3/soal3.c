#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>

//pthread_t tid[2];
//pid_t child;

pthread_t tid[2];

/*
void add_file(char filename[], char foldername[])
{
	int i = 0;
	while(&filename[i] != NULL)
	{
		foldername[i] = filename[i];
		i++;
	}
}*/

void* iniinput(void *arg)
{
	char directory[PATH_MAX];
	if(getcwd(directory, sizeof(directory)) != NULL)
	printf("%s\n", directory); // /home/dewanggad99
	pthread_t id = pthread_self();
	char **argv = arg;
	if(pthread_equal(id, tid[0]))
	{
		int i = 2, j, num = 0;
		char garing = '/';
		while(argv[i] != NULL)
		{
			j = 2;
			char extension[PATH_MAX] = {};
			//char extension[PATH_MAX];
			while(argv[i][j] != '.')
			{
				j++;
			}
			strncat(extension, &garing, 1);
			strncat(extension, &argv[i][j + 1], PATH_MAX);
			strncat(directory, extension, PATH_MAX);
			printf("File %d %s\n", i - 1, directory);
			i++;
		}
		
	}
	else if(pthread_equal(id, tid[1]))
	{
		printf("Yeah baby tid1\n");
	}
	else if(pthread_equal(id, tid[2]))
	{
		printf("Yeah baby tid2\n");
	}
	return NULL;
}

int main(int argc, char **argv)
{
	int files = argc - 2, flag, i = 0;
	if(strcmp(argv[1], "-f") == 0)
	{
		if(files == 0)
		printf("Input file kosong\n");
		while(i < files)
		{
			flag = pthread_create(&(tid[0]), NULL, iniinput, argv);
			if(flag == 0)
			printf("File %d : Berhasil Dikategorikan\n", i + 1);
			else
			printf("File %d : Sad, gagal :(\n", i + 1);
			i++;
		}
	}
	else if(strcmp(argv[1], "-d") == 0)
	{
		if(files == 0)
		printf("Directory tidak ada\n");
		else if(files == 1)
		{
			flag = pthread_create(&(tid[1]), NULL, iniinput, argv);
			if(flag == 0)
			printf("Direktori sukses disimpan!\n");
			else
			printf("Yah, gagal disimpan :(\n");
			i++;
		}
	}
	else if(strcmp(argv[1], "*") == 0)
	{
		flag = pthread_create(&(tid[2]), NULL, iniinput, argv);
		if(flag == 0)
		printf("Berhasil dikategorikan\n");
		else
		printf("Sad, gagal :(\n");
	}
	else
	{
		printf("Input salah\n");
	}
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);
	
	return 0;
}
