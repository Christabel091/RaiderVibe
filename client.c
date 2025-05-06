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
        printf("\n---------------------\n Welcome to RaiderVibe\n-------------------\n\n If you are a new user select (1) to sign up otherwise select (2) to log in and 3 to exit\n");
        bool sucess = false;
        char Username[];
        char request[]
        do{
             printf("Select an option [1 or 2}: ");
        bzero(buffer, MAXLINE);
        Fgets(buffer, MAXLINE, stdin);
        option = atoi(buffer);
        //make sure input is valid before sending to server. 
        while (option != 1 && option != 2 && option != 3){
            printf("Please select a valid option\n");
            printf("Select an option [1, 2 or 3}: ");
            bzero(buffer, MAXLINE);
            Fgets(buffer, MAXLINE, stdin);
            option = atoi(buffer);
        }

        // Send the option to the server.
        Write(clientfd, buffer, strlen(buffer));
        //ask for user's username based on if he wants to login or signup
        if (option == 1){
            //if user is signing up
            printf("Enter a new Username: ");
            bzero(username, MAXLINE);
            Fgets(username, MAXLINE, stdin);
            Write(clientfd, username, strlen(username));
            bzero(buffer, MAXLINE);
            Read(clientfd, buffer, MAXLINE);
            success = buffer;
            if (!success){
                printf("An error occoured due to usernam already exxist or bad usrname. Please try another username");
            }
            else{
                printf("Successfully signed up\n");
                printf("Commands: \n Enter \n post — create a new post
                        list — show latest posts
                        comment — comment on a post
                        like — like a post
                        search — keyword search
                        logout — log out\n");
               
            }
        }else if (option == 2){
            //if user is logging in.
            printf("Enter a Usrname: ");
            bzero(username, MAXLINE);
            Fgets(username, MAXLINE, stdin);
            Write(clientfd, username, strlen(username));
            bzero(buffer, MAXLINE);
            Read(clientfd, buffer, MAXLINE);
            sucess = buffer;
             if (!success){
                printf("Username does not exist. please verify username and try agin o signup");
            }else{
                printf("Successfully logged in\n");
                

            }
        }
        }while (!sucess);
        //execute request of user whether it is creating a posr, liking or searching up. 
        printf("%s@RaiderVibe>", username);
        Fgets(request, MAXLINE, stdin);
        //change string to lower case before comaprism
        for ( ; *request; ++request ) {
        *request = (char)tolower((unsigned char)*request);
        }
        if(strcmp(request, "post") == 0){
            Write(clientfd, request, strlen(request));
            printf("Create a new post(Enter a max of 500 characters): ");
        }else if(strcmp(request, "like") == 0){

        }else if(strcmp(request, "comment") == 0){

        }else if(strcmp(request, "search") == 0){

        }else if(strcmp(request, "log-out") == 0){

        }else{

        }
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
