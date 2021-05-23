.#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define PORT 8080
#define SIZE 1024

int connection = 0;
int currentConnection = 0;
int queue = 0;
int id_socket[1000];

void *handleLogReg(void *args);
void handleSecondPhase(int sock, char *id, char *password);
void handleStopConnection(int sock);
void write_file(int sockfd, char *filename);
void send_file(FILE *fp, int sockfd);

int main(int argc, char const *argv[])
{
    // CREATE FILES FOLDER
    mkdir("FILES", 0777);

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    printf("running on port %d\n", PORT);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    pthread_t thread[50];

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        id_socket[currentConnection] = new_socket;

        if (connection > 0)
        {
            printf("📩 sending wait signal to currentConnection: %d\n",
                   currentConnection);
            send(id_socket[currentConnection], "wait", strlen("wait"), 1024);

            pthread_create(&thread[currentConnection], NULL, handleLogReg, &new_socket);
        }
        else
        {
            printf("📩 sending go signal to currentConnection: %d\n",
                   currentConnection);
            send(id_socket[currentConnection], "go", strlen("go"), 1024);

            pthread_create(&thread[currentConnection], NULL, handleLogReg, &new_socket);
        }
        connection++;
        currentConnection++;
    }

    return 0;
}

void *handleLogReg(void *args)
{
    int new_socket = *(int *)args;
    int valread;

    char buffer[1024] = {0};
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);

    // TODO REMOVE TEMPORARY STOP
    if (strcmp(buffer, "stop") == 0)
    {
        printf(
            "--- [handleLogReg()] just got stop signal, proceeding "
            "handleStopConnection()\n");
        handleStopConnection(new_socket);
    }

    if (strcmp(buffer, "login") == 0 || strcmp(buffer, "register") == 0)
    {
        char id[1024] = {0};
        valread = read(new_socket, id, 1024);
        char password[1024] = {0};
        valread = read(new_socket, password, 1024);

        printf("id: %s, password: %s\n", id, password);

        // if register then write the id and username to the akun.txt
        if (strcmp(buffer, "register") == 0)
        {
            FILE *f;
            f = fopen("akun.txt", "a+");
            fprintf(f, "%s:%s\n", id, password);

            char authMessage[100];
            sprintf(authMessage, "2");
            send(new_socket, authMessage, strlen(authMessage), 0);

            fclose(f);

            handleLogReg(&new_socket);
        }

        if (strcmp(buffer, "login") == 0)
        {
            FILE *f;
            f = fopen("akun.txt", "a+");
            char buffer[1024] = "";
            int isValid = 0;
            // While masih ada yang bisa diread, dan selagi masih belom valid (belom
            // nemu yang id password sama)
            while (fgets(buffer, 1024, f) != NULL && !isValid)
            {
                char compare_id[1025], compare_password[1025];

                // tokenize with ':' as a delimiter
                char *token = strtok(buffer, ":");
                strcpy(compare_id, token);

                // get next token until it meets '\n'
                token = strtok(NULL, "\n");
                strcpy(compare_password, token);

                if (strcmp(compare_id, id) == 0 && strcmp(compare_password, password) == 0)
                {
                    isValid = 1;
                }
            }

            char authMessage[500];
            sprintf(authMessage, "%d", isValid);
            send(new_socket, authMessage, strlen(authMessage), 0);
            fclose(f);

            if (isValid)
            {
                handleSecondPhase(new_socket, id, password);
            }
            else
            {
                handleLogReg(&new_socket);
            }
        }
    }
    else
    {
        handleLogReg(&new_socket);
        pthread_cancel(pthread_self());
    }
}

void handleSecondPhase(int sock, char *id, char *password)
{
    // WAITING FOR MENU
    int valread;
    char buffer[1024] = {0};
    valread = read(sock, buffer, 1024);
    printf("--- [handleSecondPhase()] first command: %s\n", buffer);

    // TODO REMOVE TEMPORARY STOP
    if (strcmp(buffer, "stop") == 0)
    {
        printf(
            "--- [handleSecondPhase()] just got stop signal, proceeding "
            "handleStopConnection()\n");
        handleStopConnection(sock);
    }

    if (strcmp(buffer, "add") == 0)
    {
        printf("--- [handleSecondPhase()] add signal\n");
        printf(
            "---  [handleSecondPhase()] Preserve id:password for running.log%s:%s\n",
            id, password);

        // TODO 1. Valread file name dll
        char publikasi[120] = {0};
        valread = read(sock, publikasi, 1024);
        char tahunPublikasi[120] = {0};
        valread = read(sock, tahunPublikasi, 1024);
        char filename[120] = {0};
        valread = read(sock, filename, 1024);

        // TODO 2. Write to files.tsv
        FILE *fp;
        fp = fopen("files.tsv", "a+");
        fprintf(fp, "%s\t%s\t%s\n", publikasi, tahunPublikasi, filename);
        fclose(fp);

        // TODO 3. Write to running.log
        fp = fopen("running.log", "a+");
        // FILES/ get cut off
        char *filenameWithoutFolder = filename + 6;
        fprintf(fp, "Tambah: %s (%s:%s)\n", filenameWithoutFolder, id, password);
        fclose(fp);

        // TODO 4. Write file yang dikirim by line oleh client
        write_file(sock, filename);
        printf("🥳 Data written in the file successfully.\n");
    }
    // If not the command above, then ask again
    handleSecondPhase(sock, id, password);
}

void handleStopConnection(int sock)
{
    connection--;

    // ambil connection sebelumnya
    // currentConnection--;
    // go to next connection in queue
    queue++;
    printf("connection:  %d\n", connection);
    printf("currentConnection:  %d\n", currentConnection);
    printf("queue:  %d\n", queue);

    printf("🚥 sending signal to queue: %d is closed to next connection\n", queue);
    send(id_socket[queue], "go", strlen("go"), 1024);

    // Reset count connection and use the available thread;
    if (queue == currentConnection)
    {
        queue = 0;
        currentConnection = 0;
        printf("🕓 currentConnection:  %d\n", currentConnection);
        printf("🕓 queue:  %d\n", queue);
    }

    pthread_cancel(pthread_self());
}

// Ask for file path first
void write_file(int sockfd, char *filename)
{
    int n;
    FILE *fp;
    char buffer[SIZE];

    printf("--- [write_file] File to be written in server: %s\n", filename);

    fp = fopen(filename, "w");
    bzero(buffer, SIZE);
    while (1)
    {
        n = recv(sockfd, buffer, SIZE, 0);

        // Kalo yang ngirim bukan dari send_file (karena dari function send_file
        // pasti 1024)
        if (n != 1024)
        {
            break;
            return;
        }

        // masukkin ke filenya
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    fclose(fp);
    return;
}

void send_file(FILE *fp, int sockfd)
{
    int n;
    char data[1024] = {0};

    while (fgets(data, 1024, fp) != NULL)
    {
        if (send(sockfd, data, sizeof(data), 0) == -1)
        {
            perror("[-]Error in sending file.");
            exit(1);
        }
        bzero(data, 1024);
    }

    // mark last with ending
    send(sockfd, "stop signal", sizeof("stop signal"), 0);
}
