#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define HashTableSize 128

struct linked_list{
    char *name;
    int name_length;
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
    char *name = malloc(length);
    int i;
    for(i = 0; i < length; i++){
        name[i] = buffer[i];
    }
    return name;
}

int getSocketDescriptor(char *name, int name_length, struct linked_list *Accounts[]){
    int sd = -1;
    int hashname;
    int i;

    for(i = 0; i < name_length; i++){
        hashname += (int)name[i];
    }
    struct linked_list *Entry = Accounts[hashname % HashTableSize]; 
    if(Entry == NULL){
        printf("5.1.0 Entry == NULL\n");
        printf("hashname = %d\n", hashname);
        return -1;
    }
    if(strcmp(Entry->name, name) == 0){
        sd = Entry->SocketDescriptor;
    } else {
        while(Entry->Next != NULL){
            Entry = Entry->Next;
            if(strcmp(Entry->name, name) == 0){
                sd = Entry->SocketDescriptor;
                break;
            }   
        }
    }
    return sd;
}

void communicating(char *from, int name_length, int fd, struct linked_list *Accounts[]){
    int connected = 1;
    char buffer[256];
    int sd;

    int name_read = read(fd, buffer, sizeof(buffer));
    char *name = parseName(buffer, name_read - 1);
    sd = getSocketDescriptor(name, name_read - 1, Accounts);
    if(sd == -1){
        write(fd, "USER OFFLINE", 12);
    }
    write(sd, from, name_length); // who sent the message
    int bytes_read = read(fd, buffer, sizeof(buffer));
    write(sd, buffer, bytes_read); // the message

}


int addOnline(int sd, char *name, int name_length, struct linked_list* Accounts[]){
    struct linked_list *new = malloc(sizeof(struct linked_list));
    new->SocketDescriptor = sd;
    new->name = name;
    new->name_length = name_length;
    int hashname = 0;
    int i;
    for(i = 0; i < name_length; i++){
        hashname += (int)name[i];
    }
    printf("hashname = %d\n", hashname);
    
    struct linked_list *Entry = Accounts[hashname % HashTableSize];

    if(!Entry){
        Accounts[hashname % HashTableSize] = new;
    } else {
        if(strcmp(name, Entry->name) == 0){
            return 0;
        }
        while(Entry->Next != NULL){
            Entry = Entry->Next;
            if(strcmp(name, Entry->name) == 0){
                return 0;
            }
        }
        Entry->Next = new;
    }
    return 1;
}


int main(){
    struct sockaddr_in addr, server;
    int clilen = sizeof(server);
    int ready_sockets; // Number of sockets that request service.
    int num_sockets; // Number of connected sockets including master socket.
    fd_set read_sd; // Set of sockets. needs to be redone every iteration.
    int i;
    //printf("HashTableSize = %d\n", HashTableSize);
    int sd = initialize_TCPsocket(&addr);

    struct linked_list *Accounts[HashTableSize] = {NULL};

    char buffer[256];
    int server_fd;
    char *name;
    while(1){

        FD_ZERO(&read_sd); // clear the set.
        FD_SET(sd, &read_sd); // Add master socket to set.
        num_sockets = sd;

        for(i = 0; i < HashTableSize; ++i){
            struct linked_list *Entry = Accounts[i];
            while(Entry != NULL){
                FD_SET(Entry->SocketDescriptor, &read_sd); // Add all the sockets to set.
                if(Entry->SocketDescriptor > num_sockets){
                    num_sockets = Entry->SocketDescriptor;
                }
                Entry = Entry->Next;
            }
        }
        // Find all the "Active" sockets. 
        printf("Test\n");
        ready_sockets = select(num_sockets + 1, &read_sd, NULL, NULL, NULL);
        printf("number of ready sockets = %d\n", ready_sockets);
        // If master socket is "Active" a new connection is incomming.
        if(FD_ISSET(sd, &read_sd)){
            server_fd = accept(sd, (struct sockaddr *)&server, &clilen);
            if(server_fd == -1){
                perror("accept");
            } else {
                printf("Client is trying to connect\n");
                int read_name = read(server_fd, buffer, 11);
                name = parseName(buffer, read_name - 1);
                int result = addOnline(server_fd, name, read_name - 1, Accounts);

                if(result == 0){
                    write(server_fd, "ERROR", 5);
                } else {
                    write(server_fd, "SUCCESS", 7);
                }
            }
        }

        // Go through all the sockets and se if they belong to the set. (Is it possible to iterate over read_sd_)
        for(i = 0; i < HashTableSize; ++i){
            struct linked_list *Entry = Accounts[i];
            while(Entry != NULL){
                if(FD_ISSET(Entry->SocketDescriptor, &read_sd)){
                    communicating(Entry->name, Entry->name_length, Entry->SocketDescriptor, Accounts); // Handle the sockets request.
                }
                Entry = Entry->Next;
            }
        }
    }
    close(server_fd);
    close(sd);
}