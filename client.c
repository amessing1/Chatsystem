#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


int main(int argc, char* argv[]){

    char *Server_Address = "127.0.0.1";
    uint16_t port = atoi(argv[1]);
    char buffer[256];

    int socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket_descriptor == -1){
        perror("socket descriptor");
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(Server_Address);
    
    if(connect(socket_descriptor, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
        perror("connecting");
    }
    printf("write message:\n");
    int bytes_read = read(0, buffer, sizeof(buffer));
    write(1, buffer, bytes_read);
    write(socket_descriptor, buffer, bytes_read);
    close(socket_descriptor);
}