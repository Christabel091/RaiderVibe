#include "csapp.h"

int main(int argc, char *argv[])
{
    int clientfd;
    char *host, *port;
    char buffer[MAXLINE];
    int option = 0;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    host = argv[1];
    port = argv[2];
    
    clientfd = Open_clientfd(host, port);
    // welcome user.
        printf("\n---------------------\n Welcome to RaiderVibe\n-------------------\n\n If you are a new user select (1) to sign up otherwise select (2) to log in \n");
        printf("Select an option [1 or 2}: ");
        bzero(buffer, MAXLINE);
        Fgets(buffer, MAXLINE, stdin);
        option = atoi(buffer);
        //make sure input is valid before sending to server. 
        while (option != 1 && option != 2){
            printf("Please select a valid option\n");
            printf("Select an option [1 or 2}: ");
            bzero(buffer, MAXLINE);
            Fgets(buffer, MAXLINE, stdin);
            option = atoi(buffer);
        }

        // Send the option to the server.
        Write(clientfd, buffer, strlen(buffer));
         // Read and display the server's response whee user is successfully logges in or signed up.
        bzero(buffer, MAXLINE);
        Read(clientfd, buffer, MAXLINE);
        printf("Message from Server: %s\n", buffer);
    // Client interaction loop.
    while (1) {
        
        
        }
        
        // Read and display the server's response.
        bzero(buffer, MAXLINE);
        Read(clientfd, buffer, MAXLINE);
        printf("Message from Server: %s\n", buffer);
    }
    
    Close(clientfd);
    return 0;
}
