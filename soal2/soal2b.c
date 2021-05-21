#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include<sys/wait.h>

//dijadikan publik, biar gk sulit pas passing argumen thread
// Handle matriks result hasil faktorial
long long int result[4][6];

//Handle matriks input terbaru
int b[4][6];

//Handle matriks shared memory;
int (*a)[6];
int aTemp[4][6];


//Struct untuk simpan argumen thread
struct arg_struct{
    int baris;
    int kolom;
};

void isiMatriks1(int (*b)[6]){
    int i, j;
    printf("Masukkan matriks 4 x 6 : \n");
    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){
            scanf("%d", &b[i][j]);
        }
    }
}

long long int factCus(int i, int j){
    unsigned long long hasil = 1, hasilPembagi = 1;
    int k;

    //Kasus 0
    if(aTemp[i][j] == 0 || b[i][j] == 0){
        return 0;
    }

    //a!
    for(k = aTemp[i][j]; k >= 1; k--){
        hasil *= k;
    } 

    // Kasus a!/(a-b)!
    if(aTemp[i][j] >= b[i][j]){
        for(k = aTemp[i][j] - b[i][j]; k >= 1; k--){
            hasilPembagi *= k;
        }
        return (hasil / hasilPembagi);
    }

    //kasus a!
    else if(aTemp[i][j] < b[i][j]){
        return hasil;
    }

    // int hasil = 1;
    // for(temp = 0; temp < b[i][j]; temp++){
    //     hasil *= (aTemp[i][j] - temp);
    // }

    // return hasil; 
}

void *factorialCustom(void *argumen){
    int i = ((struct arg_struct *)argumen)->baris;
    int j = ((struct arg_struct *)argumen)->kolom;
    result[i][j] = factCus(i, j);  
}

void printHasil(){
    int i,j;
    printf("Hasil perkaliannya adalah : \n");
    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){
            printf("%lld ", result[i][j]);
        }
        printf("\n");
    } 
}

int main(){
    key_t key = 1234;

    //Buat segment shared memorynya (return identifiernya)
    int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
    a = shmat(shmid, 0, 0);

    memcpy(aTemp, a, sizeof aTemp);

    shmdt(a);
    shmctl(shmid, IPC_RMID, NULL);

    isiMatriks1(b);

    pthread_t tid[4][6];

    int i, j;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){
            struct arg_struct *argumen = (struct arg_struct *)malloc(sizeof(struct arg_struct));
            argumen->baris = i;
            argumen->kolom = j;
            pthread_create(&tid[i][j], NULL, &factorialCustom, (void *)argumen);
        }
    }

    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){
            pthread_join(tid[i][j], NULL);
        }
    }

    printHasil();
}