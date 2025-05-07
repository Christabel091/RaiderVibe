#include "csapp.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void strip_nl(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

int main(int argc, char *argv[])
{
    int clientfd;
    char *host, *port;
    char buffer[MAXLINE];
    int option = 0;
    char username[MAXLINE];
    char request[MAXLINE];
    bool success = false;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    host = argv[1];
    port = argv[2];
    clientfd = Open_clientfd(host, port);

    printf("\n---------------------\n Welcome to RaiderVibe\n---------------------\n\n");
    printf("If you are a new user select (1) to sign up, (2) to log in, or (3) to exit\n");

    do {
        printf("Select an option [1, 2 or 3]: ");
        bzero(buffer, MAXLINE);
        Fgets(buffer, MAXLINE, stdin);
        option = atoi(buffer);

        while (option != 1 && option != 2 && option != 3) {
            printf("Invalid option. Try again [1, 2 or 3]: ");
            bzero(buffer, MAXLINE);
            Fgets(buffer, MAXLINE, stdin);
            option = atoi(buffer);
        }

        if (option == 3) {
            printf("Goodbye!\n");
            Close(clientfd);
            return 0;
        }

        Write(clientfd, buffer, strlen(buffer));  // send option

        printf("Enter your username: ");
        bzero(username, MAXLINE);
        Fgets(username, MAXLINE, stdin);
        strip_nl(username);
        Write(clientfd, username, strlen(username));

        bzero(buffer, MAXLINE);
        Read(clientfd, buffer, MAXLINE);

        if (strncmp(buffer, "OK", 2) == 0) {
            success = true;
            printf("Successfully %s\n", option == 1 ? "signed up" : "logged in");
            printf("Commands: post | list | comment | like | search | logout\n");
        } else {
            printf("Error: %s\n", buffer);
        }

    } while (!success);

    while (true) {
        printf("%s@RaiderVibe> ", username);
        bzero(request, MAXLINE);
        Fgets(request, MAXLINE, stdin);
        strip_nl(request);

        // convert to lowercase
        for (int i = 0; request[i]; i++) {
            request[i] = tolower(request[i]);
        }

        Write(clientfd, request, strlen(request));

        if (strcmp(request, "post") == 0) {
            char post[1000];
            printf("Create a new post (max 1000 characters): ");
            Fgets(post, sizeof(post), stdin);
            Write(clientfd, post, strlen(post));
            bzero(buffer, MAXLINE);
            Read(clientfd, buffer, MAXLINE);
            printf("%s\n", buffer);

        } else if (strcmp(request, "like") == 0) {
            printf("Enter post ID to like: ");
            bzero(buffer, MAXLINE);
            Fgets(buffer, MAXLINE, stdin);
            strip_nl(buffer);
            Write(clientfd, buffer, strlen(buffer));
            bzero(buffer, MAXLINE);
            Read(clientfd, buffer, MAXLINE);
            printf("%s\n", buffer);

        } else if (strcmp(request, "comment") == 0) {
            printf("Enter post ID to comment on: ");
            bzero(buffer, MAXLINE);
            Fgets(buffer, MAXLINE, stdin);
            strip_nl(buffer);
            Write(clientfd, buffer, strlen(buffer));

            printf("Enter your comment: ");
            bzero(buffer, MAXLINE);
            Fgets(buffer, MAXLINE, stdin);
            strip_nl(buffer);
            Write(clientfd, buffer, strlen(buffer));

            bzero(buffer, MAXLINE);
            Read(clientfd, buffer, MAXLINE);
            printf("%s\n", buffer);

        } else if (strcmp(request, "search") == 0) {
            char keyword[50];
            printf("Enter keyword to search: ");
            bzero(keyword, sizeof(keyword));
            Fgets(keyword, sizeof(keyword), stdin);
            strip_nl(keyword);
            Write(clientfd, keyword, strlen(keyword));

            bzero(buffer, MAXLINE);
            Read(clientfd, buffer, MAXLINE);
            int count = atoi(buffer);

            if (count <= 0) {
                printf("No posts found.\n");
            } else {
                for (int i = 0; i < count; i++) {
                    printf("[Post #%d] ", i + 1);
                    bzero(buffer, MAXLINE);
                    Read(clientfd, buffer, MAXLINE); // username
                    printf("%s ", buffer);
                    bzero(buffer, MAXLINE);
                    Read(clientfd, buffer, MAXLINE); // time
                    printf("(%s)\n", buffer);
                    bzero(buffer, MAXLINE);
                    Read(clientfd, buffer, MAXLINE); // content
                    printf("  %s\n\n", buffer);
                }
            }

        } else if (strcmp(request, "list") == 0) {
            bzero(buffer, MAXLINE);
            Read(clientfd, buffer, MAXLINE);
            int count = atoi(buffer);

            if (count == 0) {
                bzero(buffer, MAXLINE);
                Read(clientfd, buffer, MAXLINE);
                printf("%s\n", buffer);
            } else {
                for (int i = 0; i < count; i++) {
                    printf("[Post #%d] ", i + 1);
                    bzero(buffer, MAXLINE);
                    Read(clientfd, buffer, MAXLINE); // username
                    printf("%s ", buffer);
                    bzero(buffer, MAXLINE);
                    Read(clientfd, buffer, MAXLINE); // time
                    printf("(%s)\n", buffer);
                    bzero(buffer, MAXLINE);
                    Read(clientfd, buffer, MAXLINE); // post
                    printf("  %s\n\n", buffer);
                }
            }

        } else if (strcmp(request, "logout") == 0) {
            printf("%s, you are successfully logged out. See you soon!\n", username);
            break;

        } else {
            printf("Invalid command. Try again.\n");
        }
    }

    Close(clientfd);
    return 0;
}
