#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/stat.h>
#include <ctype.h>

//pthread_t tid[2];
//pid_t child;

pthread_t tid[PATH_MAX];

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

void *iniinput(void* argv)
{
	char *jalan = (char*)argv, garing = '/';
	char extension[PATH_MAX] = {},
	filename1[PATH_MAX] = {}, filename2[PATH_MAX] = {},
	destination[PATH_MAX] = {},
	directory[PATH_MAX] = {},
	*unknown = "Unknown",
	*hidden = "Hidden",
	*blank = "Blank";
	printf("%s\n", jalan);
	getcwd(directory, sizeof(directory));
	strcpy(destination, directory);
	strncat(destination, &garing, 1);
	int i = 0, j;
	while(jalan[i] != '\0' && jalan[i] != '.')
	{
		i++;
	}
	if(jalan[i - 1] == '/')
	{
		if(jalan[i] == '.')
		{
			strcat(destination, hidden);
			printf("%s\n", destination);
		}
		else if(jalan[i] == '\0')
		{
			printf("Ini direktori\n");
		}
		
	}
	else if(jalan[i] == '\0')
	{
		strcat(destination, unknown);
		printf("%s\n", destination);
	}
	else if(jalan[i] == '.')
	{
		if(jalan[i + 1] == '\0')
		{
			strcat(destination, blank);
			printf("%s\n", destination);
		}
		else
		{
			i = i + 1;
			while(jalan[i] != '\0')
			{
				strncat(extension, &jalan[i], 1);
				i++;
			}
			strcat(destination, extension);
			strncat(destination, &garing, 1);
			strcat(filename1, strrchr(jalan, '/'));
			//*filename1 = strrchr(jalan, '/');
			j = 0;
			while(filename1[j] != '\0' && filename1[j] != '.')
			{
				filename2[j] = filename1[j + 1];
				j++;
			}
			filename2[j] = '\0';
			strcat(destination, filename2);
			printf("%s\n", destination);
			printf("%s\n", extension);
			printf("%s %s\n", filename1, filename2);
		}
	}
	memset(destination, 0, sizeof(destination));
	memset(extension, 0, sizeof(extension));
	memset(filename1, 0, sizeof(filename1));
	memset(filename2, 0, sizeof(filename2));
}

int main(int argc, char **argv)
{
	//printf("%s\n", argv[argc]);
	int files = argc - 2, flag, i = 0;
	if(argv[1] == NULL)
	{
		printf("Tidak ada command\n");
		return 0;
	}
	if(strcmp(argv[1], "-f") == 0)
	{
		if(files == 0)
		printf("Input file kosong\n");
		else
		{
			while(i < files)
			{
				printf("%s\n", argv[i + 2]);
				flag = pthread_create(&(tid[i]), NULL, &iniinput, (void*)argv[i + 2]);
				if(flag == 0)
				printf("File %d : Berhasil Dikategorikan\n", i + 1);
				else
				printf("File %d : Sad, gagal :(\n", i + 1);
				i++;
			}
			i = 0;
			while(i < files)
			{
				pthread_join(tid[i], NULL);
				i++;
			}
		}
	}
	
	else if(strcmp(argv[1], "-d") == 0)
	{
		/*if(files == 0)
		printf("Directory tidak ada\n");
		else if(files == 1)
		{
			flag = pthread_create(&(tid[1]), NULL, iniinput, argv);
			if(flag == 0)
			printf("Direktori sukses disimpan!\n");
			else
			printf("Yah, gagal disimpan :(\n");
			i++;
		}*/
	}
	else if(strcmp(argv[1], "*") == 0)
	{
		/*flag = pthread_create(&(tid[2]), NULL, iniinput, argv);
		if(flag == 0)
		printf("Berhasil dikategorikan\n");
		else
		printf("Sad, gagal :(\n");*/
	}
	else
	{
		printf("Input salah\n");
	}
	return 0;
}
