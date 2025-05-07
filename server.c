// server.c
#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <ctype.h>

#define MAXLINE      1024
#define MAXCLIENTS   100
#define MAX_USERNAME 64
#define TAIL_LINES   10

static int client_socks[MAXCLIENTS];
static int client_count = 0;
static pthread_mutex_t clients_mtx = PTHREAD_MUTEX_INITIALIZER;
static sem_t wlock;

void strip_nl(char *s) {
    while (*s) {
        if (*s=='\n' || *s=='\r') *s='\0';
        s++;
    }
}

void add_client(int fd) {
    pthread_mutex_lock(&clients_mtx);
    if (client_count < MAXCLIENTS)
        client_socks[client_count++] = fd;
    pthread_mutex_unlock(&clients_mtx);
}

void remove_client(int fd) {
    pthread_mutex_lock(&clients_mtx);
    for (int i=0; i<client_count; i++) {
        if (client_socks[i] == fd) {
            client_socks[i] = client_socks[--client_count];
            break;
        }
    }
    pthread_mutex_unlock(&clients_mtx);
}

void broadcast_msg(const char *msg, int exclude_fd) {
    pthread_mutex_lock(&clients_mtx);
    for (int i=0; i<client_count; i++) {
        if (client_socks[i] != exclude_fd)
            Write(client_socks[i], msg, strlen(msg));
    }
    pthread_mutex_unlock(&clients_mtx);
}

int generate_unique_postID() {
    int id;
    FILE *f;
    char line[32];
    bool found;
    do {
        id = (rand()%9000000) + 1000000;
        found = false;
        if ((f = fopen("postid.txt","r"))) {
            while (fgets(line,sizeof(line),f)) {
                if (atoi(line)==id) { found=true; break; }
            }
            fclose(f);
        }
    } while(found);
    sem_wait(&wlock);
    if ((f = fopen("postid.txt","a"))) {
        fprintf(f,"%d\n",id);
        fclose(f);
    }
    sem_post(&wlock);
    return id;
}

void store_comment(const char *post_id, const char *user, const char *text) {
    sem_wait(&wlock);
    FILE *f = fopen("comments.txt","a");
    if (f) {
        time_t t=time(NULL);
        char ts[32]; strftime(ts,sizeof(ts),"%Y-%m-%d %H:%M:%S",localtime(&t));
        fprintf(f,"%s|%s|%s|%s\n",post_id,user,ts,text);
        fclose(f);
    }
    sem_post(&wlock);
}

void store_like(const char *post_id, const char *user) {
    sem_wait(&wlock);
    FILE *f = fopen("likes.txt","a");
    if (f) {
        time_t t=time(NULL);
        char ts[32]; strftime(ts,sizeof(ts),"%Y-%m-%d %H:%M:%S",localtime(&t));
        fprintf(f,"%s|%s|%s\n",user,post_id,ts);
        fclose(f);
    }
    sem_post(&wlock);
}

void *server_thread(void *vargp) {
    int connfd = *((int*)vargp);
    free(vargp);
    add_client(connfd);

    char buf[MAXLINE], user[MAX_USERNAME];
    int n;

    // Authentication
    Read(connfd, buf, MAXLINE); strip_nl(buf);
    int opt = atoi(buf);
    Read(connfd, user, MAX_USERNAME); strip_nl(user);
    if (opt==1) {
        // Register
        sem_wait(&wlock);
        FILE *f = fopen("users.txt","r+");
        bool exists=false;
        if (f) {
            char line[MAXLINE];
            while(fgets(line,MAXLINE,f)) { strip_nl(line); if(strcmp(line,user)==0) {exists=true; break;} }
            if (!exists) fprintf(f,"%s\n",user);
            fclose(f);
        }
        sem_post(&wlock);
        if (exists) Write(connfd,"[ERROR] Username taken.\n",25);
        else       Write(connfd,"[OK] Username registered.\n",25);
        if (exists) { remove_client(connfd); Close(connfd); return NULL; }
    } else {
        // Login
        sem_wait(&wlock);
        FILE *f = fopen("users.txt","r");
        bool found=false;
        if (f) {
            char line[MAXLINE];
            while(fgets(line,MAXLINE,f)) { strip_nl(line); if(strcmp(line,user)==0){found=true;break;} }
            fclose(f);
        }
        sem_post(&wlock);
        if (!found) { Write(connfd,"[ERROR] Username not found.\n",27); remove_client(connfd); Close(connfd); return NULL; }
        Write(connfd,"[OK] Logged in.\n",16);
    }

    // Command loop
    while ((n=Read(connfd,buf,MAXLINE))>0) {
        strip_nl(buf);
        if (strcmp(buf,"post")==0) {
            Read(connfd,buf,MAXLINE); strip_nl(buf);
            int pid = generate_unique_postID();
            sem_wait(&wlock);
            FILE *f = fopen("posts.txt","a");
            if (f) {
                fprintf(f,"%s|%07d|%s\n",user,pid,buf);
                fclose(f);
            }
            sem_post(&wlock);
            char ack[MAXLINE]; snprintf(ack,sizeof(ack),"[OK] Post #%07d submitted.\n",pid);
            Write(connfd,ack,strlen(ack));
            char notif[MAXLINE]; snprintf(notif,sizeof(notif),"New post #%07d by %s: %s\n",pid,user,buf);
            broadcast_msg(notif,connfd);
        }
        else if (strcmp(buf,"comment")==0) {
            Read(connfd,buf,MAXLINE); strip_nl(buf);
            char post_id[32]; strcpy(post_id,buf);
            Read(connfd,buf,MAXLINE); strip_nl(buf);
            store_comment(post_id,user,buf);
            Write(connfd,"[OK] Comment added.\n",21);
            char notif[MAXLINE]; snprintf(notif,sizeof(notif),"New comment on #%s by %s: %s\n",post_id,user,buf);
            broadcast_msg(notif,connfd);
        }
        else if (strcmp(buf,"like")==0) {
            Read(connfd,buf,MAXLINE); strip_nl(buf);
            store_like(buf,user);
            Write(connfd,"[OK] Like recorded.\n",20);
        }
        else if (strcmp(buf,"search")==0) {
            Read(connfd,buf,MAXLINE); strip_nl(buf);
            FILE *f = fopen("posts.txt","r"); int cnt=0;
            while(fgets(user,MAXLINE,f)) if (strstr(user,buf)) cnt++;
            rewind(f);
            char tmp[16]; sprintf(tmp,"%d",cnt); Write(connfd,tmp,strlen(tmp));
            while(fgets(user,MAXLINE,f)) {
                if (!strstr(user,buf)) continue;
                strip_nl(user);
                char *u=strtok(user,"|"); char *d=strtok(NULL,"|"); char *p=strtok(NULL,"\0");
                Write(connfd,u,strlen(u)); Write(connfd,"\n",1);
                Write(connfd,d,strlen(d)); Write(connfd,"\n",1);
                Write(connfd,p,strlen(p)); Write(connfd,"\n",1);
            }
            fclose(f);
        }
        else if (strcmp(buf,"list")==0) {
            char lines[TAIL_LINES][MAXLINE]; int idx=0;
            FILE *f = fopen("posts.txt","r");
            while (fgets(lines[idx%TAIL_LINES],MAXLINE,f)) idx++;
            int total = idx<TAIL_LINES?idx:TAIL_LINES;
            char tmp[16]; sprintf(tmp,"%d",total); Write(connfd,tmp,strlen(tmp));
            int start = idx<TAIL_LINES?0:idx%TAIL_LINES;
            for (int i=0;i<total;i++) {
                char *l = lines[(start+i)%TAIL_LINES]; strip_nl(l);
                char *u=strtok(l,"|"); char *d=strtok(NULL,"|"); char *p=strtok(NULL,"\0");
                Write(connfd,u,strlen(u)); Write(connfd,"\n",1);
                Write(connfd,d,strlen(d)); Write(connfd,"\n",1);
                Write(connfd,p,strlen(p)); Write(connfd,"\n",1);
            }
            fclose(f);
        }
        else if (strcmp(buf,"logout")==0) break;
    }
    remove_client(connfd);
    Close(connfd);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc!=2) { fprintf(stderr,"usage: %s <port>\n",argv[0]); exit(0); }
    srand(time(NULL)); sem_init(&wlock,0,1);
    int listenfd = Open_listenfd(argv[1]);
    while (1) {
        struct sockaddr_storage cliaddr; socklen_t len=sizeof(cliaddr);
        int *connfdp = malloc(sizeof(int));
        *connfdp = Accept(listenfd,(SA*)&cliaddr,&len);
        pthread_t tid; pthread_create(&tid,NULL,server_thread,connfdp);
        pthread_detach(tid);
    }
    return 0;
}
