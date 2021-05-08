#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

int a[4][3];
int b[3][6];
int (*ans)[6];

// fungsi mengalikan
void kali(){
    int i, j, k;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){
            ans[i][j] = 0;
            for(k = 0; k < 3; k++){
                ans[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void isiMatriks1(){
    int i, j;
    printf("Masukkan matriks 4 x 3 : \n");
    for(i = 0; i < 4; i++){
        for(j = 0; j < 3; j++){
            scanf("%d", &a[i][j]);
        }
    }
}

void isiMatriks2(){
    int i, j;
    printf("Masukkan matriks 3 x 6 : \n");
    for(i = 0; i < 3; i++){
        for(j = 0; j < 6; j++){
            scanf("%d", &b[i][j]);
        }
    }
}

void printHasil(){
    int i,j;
    printf("Hasil perkaliannya adalah : \n");
    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){
            printf("%d ", ans[i][j]);
        }
        printf("\n");
    } 
}

int main(){

    isiMatriks1();

    isiMatriks2();

    // Bantuan 2B shared memory
    //Key yang bakal digunain buat shared memory
    key_t key = 1234;

    //Buat segment shared memorynya (return identifiernya)
    int shmid = shmget(key, sizeof(int [4][6]), IPC_CREAT | 0666);
    ans = shmat(shmid, NULL, 0);


    kali();
    printHasil();

    sleep(10);

    shmdt(ans);
    shmctl(shmid, IPC_RMID, NULL);
}   
