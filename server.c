#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

struct linked_list{
    char *name;
    int SocketDescriptor;
    struct linked_list *Next;
};

int initialize_TCPsocket(struct sockaddr_in *addr){
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd == -1){
        perror("socket");
    }
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(25800);
    int b = bind(sd, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
    if(b == -1){
        perror("bind");
    }
    if(listen(sd, 2) == -1){
        perror("listen");
    } else {
        printf("listening\n");
    }
    return sd;
}

char * parseName(char buffer[], int length){
    char *name = alloca(length);
    int i;
    for(i = 0; i < length; i++){
        name[i] = buffer[i];
    }
    return name;
}

int getSocketDescriptor(char *name, struct linked_list Accounts[]){
    int sd = -1;
    int hashname;
    int i;
    for(i = 0; i < 10; i++){
        hashname += (int)name[i];
    }
    struct linked_list Entry = Accounts[hashname % 100];
    if(strcmp(Entry.name, name) == 0){
        sd = Entry.SocketDescriptor;
    } else {
        while(Entry.Next != NULL){
            Entry = *Entry.Next;
            if(strcmp(Entry.name, name) == 0){
                sd = Entry.SocketDescriptor;
            }   
        }

    }

    return sd;
}

void communicating(int fd, struct linked_list Accounts[]){
    int connected = 1;
    char buffer[256];
    int sd;
    while(connected){
        int name_read = read(fd, buffer, sizeof(buffer));
        char *name = parseName(buffer, name_read);
        sd = getSocketDescriptor(name, Accounts);
        if(sd == -1){
            write(fd, "USER OFFLINE", 12);
        }
        int bytes_read = read(fd, buffer, sizeof(buffer));
        write(sd, buffer, bytes_read);
    }
}


void addOnline(int sd, char *name, struct linked_list Accounts[]){
    struct linked_list *new = malloc(sizeof(struct linked_list));
    new->SocketDescriptor = sd;
    new->name = name;
    int hashname;
    int i;
    for(i = 0; i < 10; i++){
        hashname += (int)name[i];
    }
    struct linked_list Entry = Accounts[hashname % 100];
    if(Entry.name == NULL){
        Accounts[hashname % 100] = *new;
    } else {
        while(Entry.Next != NULL){
            Entry = *Entry.Next;
        }
        Entry.Next = new;
    }
}


int main(){
    struct sockaddr_in addr, server;
    int clilen = sizeof(server);
    int sd = initialize_TCPsocket(&addr);
    struct linked_list Accounts[100];
    char buffer[256];
    int server_fd;
    while(1){
        server_fd = accept(sd, (struct sockaddr *)&server, &clilen);
        if(server_fd == -1){
            perror("accept");
        } else {
            // CONNECTED!
            int read_name = read(server_fd, buffer, 10);
            char *name = parseName(buffer, 10);
            addOnline(server_fd, name, Accounts);
            communicating(server_fd, Accounts);
        }
    }
    close(server_fd);
    close(sd);
}