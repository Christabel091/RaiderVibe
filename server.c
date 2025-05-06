// server.c
#include "csapp.h"
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>   // for str comparism

#ifndef MAX_SIZE
#define MAX_SIZE 8192
#endif

// Reader-writer synchronization primitives/variables
static sem_t mutex, wlock;
static int readcnt = 0;
// Initialize locks
void init_locks() {
    sem_init(&mutex, 0, 1);
    sem_init(&wlock, 0, 1);
}

// Destroy locks (never really called)
void destroy_locks() {
    sem_destroy(&mutex);
    sem_destroy(&wlock);
}

// Reader entry
void reader_lock() {
    sem_wait(&mutex);
    if (readcnt++ == 0)
        sem_wait(&wlock);
    sem_post(&mutex);
}

// Reader exit
void reader_unlock() {
    sem_wait(&mutex);
    if (--readcnt == 0)
        sem_post(&wlock);
    sem_post(&mutex);
}
bool searchforUser(char username[]){
    reader_lock();
     FILE *file = fopen("username.txt", "r");
    if (!file) {
        snprintf(result, MAX_SIZE, "Error opening records file.\n");
        reader_unlock();
        return result;
    }
    bool found = false;
    char User[500];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^,]", == 1)){
            if (strcmp(username, User) == 0){
                found = true;
            }
        }
    }
     fclose(file);
    reader_unlock();
    return found;

}
bool signUserUp(char username[]){
    //call search user function to check if username exists or is in the file or not
    if (searchforUser(username)){
        return false;
    }else{
         sem_wait(&wlock);
            FILE *file = fopen("username.txt", "a");
            if (!file) {
                return false;
            } else {
                fprintf(file, "%s\n",
                        username);
                fclose(file);
            }
            sem_post(&wlock);
        return true;
    }

}
bool logUserIn(char username[]){
    //call search for user function to see if username exists and user can b logged in. 
     if (searchforUser(username)){
        return true;
    }else{
        return false;
    }
}
void *serverFunction(void *vargp) {
    int connfd = *((int *)vargp);
    free(vargp);

    char buf[MAXLINE];

    //buf represent buffer, input from terminal whether to log in or sign up 
    int n = Read(connfd, buf, MAXLINE);
    if (n <= 0) // client closed
        break;
    buf[n] = '\0';
    strip_nl(buf);
    int option = atoi(buf);
    //next, log user in by checking for thier name in the file or signing them up  appending their username to the useraname.txt file 
    char username[MAX_SIZE];
     bool sucess = false;
    if (option == 1){
        //add users username into file.
        Read(connfd, username, MAXLINE);
        strip_nl(username);
        bool sucess = signUserUp(username); 
        Write(connfd, sucess, strlen(result));
        free(sucess);
    }else if (option == 2){
        //check if username is in the username text  file and exists. 
        Read(connfd, username, MAXLINE);
        strip_nl(username);
        bool sucess = logUserIn(username);
        Write(connfd, sucess, strlen(sucess));
        free(sucess);
    }else{
        const char *msg = "Successfully lpgged out\n";
        Write(connfd, msg, strlen(msg));
        return NULL;
    }
    //server has succesfully handles signup and log in
    //now handle app requests like posting, liking commenting. e.t.c
    char request[MAXLINE];
    Read(connfd, request, MAXLINE);
    //perform services for client based on input word from user like "post"
    if(strcmp(request, "post") == 0){
        
    }else if(strcmp(request, "like") == 0){

    }else if(strcmp(request, "comment") == 0){

    }else if(strcmp(request, "search") == 0){

    }else if(strcmp(request, "log-out") == 0){

    }else{

    }
        

    Close(connfd);
    return NULL;

}
int main(int argc, char *argv[]) {
    int listenfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    init_locks();
    listenfd = Open_listenfd(argv[1]);

    while (1) {
        clientlen = sizeof(clientaddr);
        int *connfdp = malloc(sizeof(int));
        *connfdp = Accept(listenfd,
                          (SA *)&clientaddr,
                          &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen,
                    client_hostname, MAXLINE,
                    client_port,   MAXLINE, 0);
        printf("Connected to (%s, %s)\n",
               client_hostname, client_port);

        pthread_t tid;
        pthread_create(&tid, NULL, serverFunction, connfdp);
        pthread_detach(tid);
    }

    destroy_locks();
    return 0;
}
