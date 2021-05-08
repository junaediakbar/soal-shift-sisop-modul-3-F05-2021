#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

// fungsi mengalikan
void kali(int a[][3], int b[][6], int (*ans)[6]){
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

void isiMatriks1(int a[][3]){
    int i, j;
    printf("Masukkan matriks 4 x 3 : \n");
    for(i = 0; i < 4; i++){
        for(j = 0; j < 3; j++){
            scanf("%d", &a[i][j]);
        }
    }
}

void isiMatriks2(int b[][6]){
    int i, j;
    printf("Masukkan matriks 3 x 6 : \n");
    for(i = 0; i < 3; i++){
        for(j = 0; j < 6; j++){
            scanf("%d", &b[i][j]);
        }
    }
}

void print(int (*result)[6]){
    int i,j;
    printf("Hasil perkaliannya adalah : \n");
    for(i = 0; i < 4; i++){
        for(j = 0; j < 6; j++){
            printf("%d ", result[i][j]);
        }
        printf("\n");
    } 
}

int main(){

    int a[4][3];
    isiMatriks1(a);

    int b[3][6];
    isiMatriks2(b);

    // Bantuan 2B shared memory
    //Key yang bakal digunain buat shared memory
    key_t key = 1234;
    int (*result)[6];

    //Buat segment shared memorynya (return identifiernya)
    int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
    result = shmat(shmid, 0, 0);
    
    kali(a, b, result);
    print(result);

    sleep(5);

    shmdt(result);
    shmctl(shmid, IPC_RMID, NULL);
}   

