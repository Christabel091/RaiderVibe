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
    if (option == 1){
        
    }else if (option == 2){
        
    }else{
        const char *msg = "Successfully lpgged out\n";
        Write(connfd, msg, strlen(msg));
        return NULL;
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
