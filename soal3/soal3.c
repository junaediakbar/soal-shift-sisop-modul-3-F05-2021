#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <libgen.h>

#define TRUE 1
#define FALSE 0
#define F 2
#define D 3
#define STAR 4
int countfile = 1;

int isDirectory(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return FALSE;
    return S_ISDIR(statbuf.st_mode);
}

int checkAllFileExist(int argc, char *argv[])
{
    int i;
    char curr_dirr[1000];
    getcwd(curr_dirr, sizeof(curr_dirr));
    strcat(curr_dirr, "/");
    for (i = 2; i < argc; i++)
    {
        char temp[1000];
        if (argv[i][0] != '/')
        {
            strcpy(temp, curr_dirr);
            strcat(temp, argv[i]);
        }
        else
            strcpy(temp, argv[i]);
        struct stat buffer;
        if (stat(argv[i], &buffer) != 0 || isDirectory(temp))
            return FALSE;
    }
    return TRUE;
}

const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";

    return dot;
}

int checkMainArg(int argc, char *argv[])
{
    if (argc < 2)
        return FALSE;
    if (argv[1][0] == '*' && strlen(argv[1]) == 1 && argc == 2)
        return STAR;
    if (argv[1][0] == '-' && strlen(argv[1]) == 2)
        switch (argv[1][1])
        {
        case 'f':
            return (argc < 3) ? FALSE : (strlen(argv[1]) != 2)         ? FALSE
                                    : (!checkAllFileExist(argc, argv)) ? FALSE
                                                                       : F;
        case 'd':
            return (argc != 3) ? FALSE : (strlen(argv[1]) != 2) ? FALSE
                                     : (!isDirectory(argv[2]))  ? FALSE
                                                                : D;
        }

    return FALSE;
}

void moveFileToNewPath(char *source, char *dest)
{
    FILE *fp1, *fp2;
    int ch;

    fp1 = fopen(source, "r");
    fp2 = fopen(dest, "w");

    while ((ch = fgetc(fp1)) != EOF)
        fputc(ch, fp2);

    fclose(fp1);
    fclose(fp2);

    remove(source);
}

void *moveFile(void *ptr)
{
    int flag = 0;
    char *param = (char *)ptr;
    const char *extension_temp = get_filename_ext(param);
    int ch, i, length = strlen(extension_temp);
    char extension[100];
    char *filename = basename(param);
    char new_path[1000], temp[1000];
    if (filename[0] == '.')
    {
        if (!isDirectory("Hidden"))
            mkdir("Hidden", 0777);
        strcpy(new_path, "Hidden/");
        strcat(new_path, filename);
        moveFileToNewPath(param, new_path);

        if (flag == 0)
        {
            printf("File %d: Berhasil Dikategorikan\n", countfile);
            countfile++;
        }
    }
    else
    {
        memset(new_path, 0, sizeof(new_path));
        memset(extension, 0, sizeof(extension));
        for (i = 1; i < length; i++)
        {
            extension[i - 1] = tolower(extension_temp[i]);
        }

        if (!strlen(extension_temp))
        {
            if (!isDirectory("Unknown"))
                mkdir("Unknown", 0777);
            strcpy(new_path, "Unknown/");
            strcat(new_path, filename);
            flag = 1;
        }
        else
        {
            if (!isDirectory(extension))
                mkdir(extension, 0777);
            strcpy(new_path, extension);
            strcat(new_path, "/");
            strcat(new_path, filename);
        }
        moveFileToNewPath(param, new_path);
        if (flag == 0)
        {
            printf("File %d: Berhasil Dikategorikan\n", countfile);
            countfile++;
        }
        else if (flag == 1)
        {
            printf("File %d: Sad,gagal :(\n", countfile);
            countfile++;
        }
    }
}
int i = 0;
void listFilesRecursively(char *basePath, int *iret, pthread_t *threads)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name);

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            char *arr = path;
            iret[i] = pthread_create(&threads[i], NULL, moveFile, (void *)arr);
            if (iret[i])
            {
                fprintf(stderr, "Error - pthread_create() return code: %d\n", iret[i]);
                exit(EXIT_FAILURE);
            }
            pthread_join(threads[i], NULL);
            i++;
            listFilesRecursively(path, iret, threads);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    int check = checkMainArg(argc, argv), iret[10000], let = 1;

    if (!check)
    {
        printf("Argumen Salah!\n");
        exit(EXIT_FAILURE);
    }

    char curr_dirr[1000];
    pthread_t threads[10000];
    struct dirent *ep;
    DIR *dp;

    switch (check)
    {
    case F:
        for (i = 2; i < argc; i++)
        {
            char *arr = argv[i];
            iret[i - 2] = pthread_create(&threads[i - 2], NULL, moveFile, (void *)arr);

            if (iret[i - 2])
            {
                fprintf(stderr, "Error - pthread_create() return code: %d\n", iret[i - 2]);
                exit(EXIT_FAILURE);
            }

            pthread_join(threads[i - 2], NULL);
        }
        break;
    case D:
        getcwd(curr_dirr, sizeof(curr_dirr));
        if (argv[2][0] != '/')
        {
            strcat(curr_dirr, "/");
            strcat(curr_dirr, argv[2]);
            if (argv[2][strlen(argv[2]) - 1] != '/')
                strcat(curr_dirr, "/");
        }
        dp = opendir(argv[2]);
        if (dp != NULL)
        {
            listFilesRecursively(argv[2], iret, threads);
        }
        break;
    case STAR:
        if (getcwd(curr_dirr, sizeof(curr_dirr)) != NULL)
        {
            dp = opendir(curr_dirr);
            if (dp != NULL)
            {
                listFilesRecursively(curr_dirr, iret, threads);
            }
        }
        break;
    }
}