#include "network_command_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP_ADDRESS> <CMD_NO> <COMMAND_VALUE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *ip_address = argv[1];
    int cmd_no = atoi(argv[2]);
    int command_value = atoi(argv[3]);

    int sockfd;
    struct sockaddr_in serv_addr;
    struct data_packet packet;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        perror("ERROR invalid IP address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    packet.start_frame = 0xAAAA;
    packet.cmd_no = cmd_no;
    packet.command_value = command_value;
    packet.end_frame = 0xBBBB;

    write(sockfd, &packet, sizeof(packet));
    close(sockfd);

    return 0;
}

