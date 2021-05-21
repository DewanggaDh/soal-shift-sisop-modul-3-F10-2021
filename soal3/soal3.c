#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/stat.h>

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
	char extension[PATH_MAX] = {}, new_fold[PATH_MAX] = {};
	char source[PATH_MAX] = {}, dest[PATH_MAX] = {}, nope[] = "unknown";
	int status;
	struct stat cek_file;
	char garing = '/';
	if(pthread_equal(id, tid[0]))
	{
		int i = 2, j, k;
		getcwd(directory, sizeof(directory));
		//printf("%s\n, directory));
		while(argv[i] != NULL)
		{
			j = 0;
			/*while(argv[i][j] != '\0' && argv[i][j] != '*')
			j++;
			if(argv[i][j] == '*')
			{
				//status = stat(argv[i], &cek_file);
				argv[i][j] = 39;
				argv[i][j + 1] = 42;
				argv[i][j + 2] = 39;
			}*/
			status = stat(argv[i], &cek_file);
			if(status != 0)
			{
				printf("File tidak ada\n");
			}
			else
			{
				printf("%s\n", argv[i]);
				if(S_ISREG(cek_file.st_mode))
				{
					while(argv[i][j] != '\0' && argv[i][j] != '.')
					j++;
					//printf("%d\n", j);
					if(argv[i][j] != '\0')
					{
						k = j + 1;
						while(argv[i][k] != '\0')
						{
							strncat(extension, &argv[i][k], 1);
							k++;
						}
						printf("%s\n", extension);
						//mkdir(extension, 0777);
						strncat(new_fold, directory, PATH_MAX);
						strncat(new_fold, &garing, PATH_MAX);
						strncat(new_fold, extension, PATH_MAX);
						//status = stat(new_fold, &cek_file);
						printf("%s\n", new_fold);
						/*
						//mkdir(new_fold, 0777);
						//mkdir("c", 0777);
						/*strncat(new_fold, &garing, PATH_MAX);
						while(j != -1 && argv[i][j] != '/')
						j -= 1;
						k = j + 1;
						while(argv[i][k] != '\0')
						{
							strncat(new_fold, &argv[i][k], 1);
							k++;
						}
						
						if(!S_ISDIR(cek_file.st_mode))
						mkdir(new_fold, 0777);
						*/
						while(j != -1 && argv[i][j] != '/')
						j -= 1;
						k = j + 1;
						strncat(new_fold, &garing, PATH_MAX);
						
						while(argv[i][k] != '\0')
						{
							strncat(new_fold, &argv[i][k], 1);
							
							printf("%c", argv[i][k]);
							k++;
						}
						printf("\n");
						printf("%s %s\n", argv[i], new_fold);
						status = rename(argv[i], new_fold);
						if(status)
						printf("yes\n");
						else
						printf("No\n");
						memset(extension, 0, sizeof(extension));
						memset(new_fold, 0, sizeof(new_fold));
				//printf("Ada ekstensi\n");
					}
					else
					printf("unknown\n");
				}
						//printf("Ini file\n");
				else if(S_ISDIR(cek_file.st_mode))
				printf("Ini folder\n");
			}
			//printf("%s\n", argv[i]);
			//j = 0;
			i++;
			
			//Ada *
			
			//printf("Tidak ada ekstencis\n");
			
			/*if(argv[i][j] - 1 == '\0')
			{
				
			}
			i++;*/
		}
		
		
		/*int status;
		struct stat cek_file;
		
		
		int i = 2, j, num = 0;
		char garing = '/';
		while(argv[i] != NULL)
		{
			j = 0;
			status = stat(argv[i], &cek_file);
			if(status != 0 || S_ISDIR(cek_file.st_mode))
			{
				printf("File %d : Sad, gagal :(\n", i - 1);
			}
			else if(S_ISREG(cek_file.st_mode))
			{
				while(argv[i][j] != '.' && argv[i][j] != '\0')
				j++;
			}
			//Cek ekstensinya
			//buat foldernya, yang sebelumnya dicek apakah sudah ada?
			//Masukan file ke dalam folder (Agak panjang)
			j = 0;
			while(argv[i][j] != '.' && argv[i][j] != '\0')
			j++;
			if(argv[i][j] == '\0' &&
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
			memset(source, 0, sizeof(source));*/
		
		
	}
	else if(pthread_equal(id, tid[1]))
	{
		printf("Yeah baby tid1\n");
		//Buka foldernya, gunakan tid ketiga, mungkin
	}
	else if(pthread_equal(id, tid[2]))
	{
		printf("Yeah baby tid2\n");
		//Gunakan yang kemarin
	}
	return NULL;
}

int main(int argc, char **argv)
{
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
		flag = pthread_create(&(tid[0]), NULL, iniinput, argv);
		/*while(i < files)
		{
			
			if(flag == 0)
			printf("File %d : Berhasil Dikategorikan\n", i + 1);
			else
			printf("File %d : Sad, gagal :(\n", i + 1);
			i++;
		}*/
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
