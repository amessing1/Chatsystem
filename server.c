#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
int main(){
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd == -1){
        perror("socket");
    }
    struct sockaddr_in addr, server;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(25800);
    int b = bind(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if(b == -1){
        perror("bind");
    }
    if(listen(sd, 2) == -1){
        perror("listen");
    } else {
        printf("listening\n");
    }
    int clilen = sizeof(server);
    int server_fd = accept(sd, (struct sockaddr *)&server, &clilen);
    if(server_fd == -1){
        perror("accept");
    } else {
        printf("connected!\n");
    }
    char buffer[256];
    read(server_fd, buffer, sizeof(buffer));
    sleep(5);
    write(1, buffer, sizeof(buffer));
    close(server_fd);
    close(sd);
}