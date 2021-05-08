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
	/*char *argv1[] = {"clear", NULL};
	pthread_t id = pthread_self();
	if(pthread_equal(id, tid[0]))
	{
		child = fork();
		if(child == 0)
		execv("/usr/bin/clear", argv1);
	}
	else if(pthread_equal(id, tid[1]))
	{
	}*/
	char directory[PATH_MAX];
	//getcwd(directory, sizeof(directory));
	//printf("%s\n", directory); // /home/dewanggad99
	
	pthread_t id = pthread_self();
	char **argv = arg;
	char extension[PATH_MAX] = {};
	char source[PATH_MAX] = {}, dest[PATH_MAX] = {}, nope[] = "unknown";
	if(pthread_equal(id, tid[0]))
	{
		int i = 2, j, num = 0;
		char garing = '/';
		while(argv[i] != NULL)
		{
			j = 2;
			getcwd(directory, sizeof(directory));
			
			//char extension[PATH_MAX];
			while(argv[i][j] != '.' && argv[i][j] != '\0')
			{
				j++;
			}
			strncat(extension, &garing, 1);
			strncat(extension, &argv[i][0], PATH_MAX);
			strncat(directory, extension, PATH_MAX);
			strncat(source, directory, PATH_MAX);
			memset(extension, 0, sizeof(extension));
			memset(directory, 0, sizeof(extension));
			getcwd(directory, sizeof(directory));
			
			strncat(extension, &garing, 1);
			if(argv[i][j] != '\0')
			{
			strncat(extension, &argv[i][j + 1], PATH_MAX);
			}
			else
			{
			strncat(extension, nope, PATH_MAX);
			}
			strncat(extension, &garing, 1);
			strncat(extension, &argv[i][0], PATH_MAX);
			strncat(directory, extension, PATH_MAX);
			strncat(dest, directory, PATH_MAX);
			printf("%s %s\n", source, dest);
			rename(source, dest);
			i++;
			memset(extension, 0, sizeof(extension));
			memset(directory, 0, sizeof(directory));
			memset(dest, 0, sizeof(dest));
			memset(source, 0, sizeof(source));
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
	//for(i = 0; i < files; i++)
	//{
	//	pthread_join(tid[i], NULL);
	//}
	//while(i <= files)
	/*int flag, i = 0;
	while(i <= argc - 2)
	{
		flag = pthread_create(a, NULL, &iniinput, argv);
	}
	if(strcmp(argv[1], "-f") == 0)
	{
	}
	else if(strcmp(argv[1], "-d") == 0)
	{
	}
	else if(strcmp(argv[1], "*") == 0)
	while(i < argc - 2 && strcmp("-f", argv[1]))
	{
	
	if(flag != 0)
	printf("Nope\n");
	else
	printf("Yes\n");
	}*/
	/*int flag, i = 0;
	while(i < 1)
	{
		flag = pthread_creat(&(tid[i]), NULL, &iniinput, 
	}*/
	return 0;
}
