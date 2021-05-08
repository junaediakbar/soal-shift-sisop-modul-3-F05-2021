# soal-shift-sisop-modul-3-F05-2021

## Soal 1
## Soal 2
## Soal 3
Rekap:
Program ini akan memindahkan file sesuai ekstensinya ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program kategori tersebut dijalankan.

Contoh apabila program dijalankan:
```
# Program soal3 terletak di /home/izone/soal3
$ ./soal3 -f path/to/file1.jpg path/to/file2.c path/to/file3.zip
#Hasilnya adalah sebagai berikut
/home/izone
|-jpg
|--file1.jpg
|-c
|--file2.c
|-zip
|--file3.zip
```
### soal 3a
Program menerima opsi -f seperti contoh di atas, jadi pengguna bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan oleh pengguna. 
Output yang dikeluarkan adalah seperti ini :
```
File 1 : Berhasil Dikategorikan (jika berhasil)
File 2 : Sad, gagal :( (jika gagal)
File 3 : Berhasil Dikategorikan
```
Perlu dilakukan inisialisasi untuk tiap kasus `-f , -d dan \*` dengan cara berikut :
```
#define TRUE 1
#define FALSE 0
#define F 2
#define D 3
#define STAR 4
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
```
Fungsi `checkMainArg(int argc, char *argv[])` juga bisa mengecek apakah argumen yang dimasukkan sudah tepat atau belum dengan menggunakan fungsi checkAllFileExist(argc, argv)
```
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
```
Selanjutnya untuk mendapatkan extension dari file dapat menggunakan cara berikut
```
const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "";

    return dot;
}
```
Selanjutnya dibuat fungsi untuk mejalankan pemindahan file-file
```
void moveFileToNewPath(char *source, char *dest)
{
    if (!isDirectory(source))
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
        work++;
    }
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
```
Selanjutnya dibuat thread untuk mejalankan pemindahan file-file 
```
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
```
### Soal 3b
Soal ini diminta untuk membuat Program yang juga dapat menerima opsi -d untuk melakukan pengkategorian pada suatu directory. Namun pada opsi -d ini, user hanya bisa memasukkan input 1 directory saja, tidak seperti file yang bebas menginput file sebanyak mungkin. Contohnya adalah seperti ini:
```
$ ./soal3 -d /path/to/directory/
```
Perintah di atas akan mengkategorikan file di /path/to/directory, lalu hasilnya akan disimpan di working directory dimana program C tersebut berjalan (hasil kategori filenya bukan di /path/to/directory).
Output yang dikeluarkan adalah seperti ini :
```
Jika berhasil, print “Direktori sukses disimpan!”
Jika gagal, print “Yah, gagal disimpan :(“
```
berikut adalah case untuk -d, untuk menentukannya dapat menggunakan fungsi `checkMainArg(int argc, char *argv[])`
```
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
            if (work == 0)
            {
                printf("Yah, gagal disimpan :(\n");
            }
            else
            {
                printf("Direktori Sukses Disimpan!\n");
            }
        }

        break;
```
Karena diminta untuk melakukan pengecekan secara rekursif, maka perlu dibuat fungsi baru yang dapat melakukannya yaitu ` listFilesRecursively(argv[2], iret, threads);`
```
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
            //printf("%s\n", dp->d_name);

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            char *arr = path;
            iret[i] = pthread_create(&threads[i], NULL, catFolder, (void *)arr);
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
```
Di dalam fungsi tersebut juga sudah terdapat thread yang akan melakukan pemindahan file
`iret[i] = pthread_create(&threads[i], NULL, catFolder, (void *)arr);`
Selanjutnya pada fungsi catFolder akan melakukan hal yang sama seperti `soal 3a` namun mengeluarkan output yang berbeda
```
void *catFolder(void *ptr)
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
    }
}
```
### soal 3c
Pada soal ini diminta untuk membuat program ini menerima opsi *, contohnya ada di bawah ini:
$ ./soal3 \*
Opsi ini akan mengkategorikan seluruh file yang ada di working directory ketika menjalankan program C tersebut.
hal yang dilakukan mirip seperti pada **soal 3c** yaitu dengan membuat case baru
```
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
```
lalu menggunakan fungsi yang sama yaitu `listFilesRecursively(curr_dirr, iret, threads);`
### Soal 3e
Pada soal ini diminta untuk Semua file harus berada di dalam folder, jika terdapat file yang tidak memiliki ekstensi, file disimpan dalam folder “Unknown”. Jika file hidden, masuk folder “Hidden”.
<br>
untuk file yang tidak dapat dikategorikan sudah dilakukan pemeriksaan pada fungsi `moveFile` dan `catFolder`
dengan  cara 
```
  if (!strlen(extension_temp))
        {
            if (!isDirectory("Unknown"))
                mkdir("Unknown", 0777);
            strcpy(new_path, "Unknown/");
            strcat(new_path, filename);
            flag = 1;
        }
```
untuk file yang tersembuyi sudah dilakukan pemeriksaan pada fungsi `moveFile` dan `catFolder` dengan mengenali nama awal dari file tersebut adalah `.`
dengan  cara 
```
 if (filename[0] == '.')
    {
        if (!isDirectory("Hidden"))
            mkdir("Hidden", 0777);
        strcpy(new_path, "Hidden/");
        strcat(new_path, filename);
        moveFileToNewPath(param, new_path);
    }
```
### Soal 3e
Pada soal ini diminta untuk setiap 1 file yang dikategorikan dioperasikan oleh 1 thread agar bisa berjalan secara paralel sehingga proses kategori bisa berjalan lebih cepat.
<br>
untuk **soal 3a** sudah dengan menggunakan 1 thread 
untuk **soal 3b** dan **soal 3c** masih belum memahami penggunaan  satu thread secara rekursif
### Kendala 
- masih belum memahami penggunaan  satu thread secara rekursif


