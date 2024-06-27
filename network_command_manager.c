#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345

struct data_packet 
{
    int command_value;
};

void forward_command_to_countdown_timer_manager(int pipe_fd, int command_value) 
{
    write(pipe_fd, &command_value, sizeof(command_value));
}

int main() 
{
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) 
    {
        perror("Error! creating pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("Error! forking process");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) 
    { 
        close(pipe_fd[1]); 
        char pipe_read_fd_str[10];
        snprintf(pipe_read_fd_str, 10, "%d", pipe_fd[0]);
        execl("./countdown_timer_manager", "countdown_timer_manager", pipe_read_fd_str, (char *)NULL);
        perror("Error! executing Countdown Timer Manager");
        exit(EXIT_FAILURE);
    } 
    else 
    { 
        close(pipe_fd[0]); 

        int server_fd, new_sockfd;
        struct sockaddr_in serv_addr, cli_addr;
        socklen_t cli_len = sizeof(cli_addr);
        struct data_packet packet;

        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) 
	{
            perror("ERROR! opening socket");
            exit(EXIT_FAILURE);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
            perror("Error! binding the socket.\n");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 5)) 
	{
            perror("Error! listening to incoming connections.\n");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        while (1) 
	{
            new_sockfd = accept(server_fd, (struct sockaddr *) &cli_addr, &cli_len);
            if (new_sockfd < 0) 
	    {
                perror("Error! accepting the connection.\n");
                close(server_fd);
                exit(EXIT_FAILURE);
            }

            read(new_sockfd, &packet, sizeof(packet));
            forward_command_to_countdown_timer_manager(pipe_fd[1], packet.command_value);
            close(new_sockfd);
        }

        close(server_fd);
    }

    return 0;
}

