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
    printf("%s\n", name);
    printf("%d\n", name_length);
    for(i = 0; i < name_length; i++){
        hashname += (int)name[i];
    }
    printf("hashname: %d\n", hashname);
    struct linked_list *Entry = Accounts[hashname % 100]; 
    if(Entry == NULL){
        printf("5.1.0 Entry == NULL\n");
    }
    printf("5.1.0\n");
    printf("Entry.name: %s,  name: %s\n", Entry->name, name);
    if(strcmp(Entry->name, name) == 0){
        printf("5.1.1\n");
        sd = Entry->SocketDescriptor;
    } else {
        printf("5.1.2\n");
        while(Entry->Next != NULL){
            Entry = Entry->Next;
            if(strcmp(Entry->name, name) == 0){
                sd = Entry->SocketDescriptor;
                printf("sd = %d\n", sd);
            }   
        }

    }

    return sd;
}

void communicating(int fd, struct linked_list *Accounts[]){
    int connected = 1;
    char buffer[256];
    int sd;
    while(connected){
        int name_read = read(fd, buffer, sizeof(buffer));
        printf("5.0\n");
        char *name = parseName(buffer, name_read - 1);
        printf("5.1\n");
        sd = getSocketDescriptor(name, name_read - 1, Accounts);
        printf("5.2\n");
        if(sd == -1){
            write(fd, "USER OFFLINE", 12);
        }
        printf("5.3\n");
        int bytes_read = read(fd, buffer, sizeof(buffer));
        printf("5.4\n");
        write(sd, buffer, bytes_read);
    }
}


int addOnline(int sd, char *name, int name_length, struct linked_list* Accounts[]){
    struct linked_list new;//malloc(sizeof(struct linked_list));
    printf("3.0\n");
    new.SocketDescriptor = sd;
    new.name = name;
    int hashname = 0;
    int i;
    printf("3.1\n");
    for(i = 0; i < name_length; i++){
        hashname += (int)name[i];
    }
    printf("hashname: %d\n", hashname);
    printf("3.2\n");
    struct linked_list *Entry = Accounts[hashname % 100];
    printf("3.3\n");

    if(Entry == NULL){
        printf("HERE");
        Accounts[hashname % 100] = &new;
    } else {
        printf("%s\n", Entry->name);
        printf("3.4\n");
        if(strcmp(name, Entry->name) == 0){
            return 0;
        }
        printf("3.5\n");
        while(Entry->Next != NULL){
            Entry = Entry->Next;
            printf("Entry.name: %s,  name: %s\n", Entry->name, name);
            if(strcmp(name, Entry->name) == 0){
                return 0;
            }
        }
        Entry->Next = &new;
    }
    printf("3.6\n");
    return 1;
}


int main(){
    struct sockaddr_in addr, server;
    int clilen = sizeof(server);
    int sd = initialize_TCPsocket(&addr);
    struct linked_list *Accounts[100] = {NULL};
    char buffer[256];
    int server_fd;
    char *name;
    while(1){
        server_fd = accept(sd, (struct sockaddr *)&server, &clilen);
        if(server_fd == -1){
            perror("accept");
        } else {
            // CONNECTED!
            printf("1\n");
            int read_name = read(server_fd, buffer, 11);
            printf("2\n");
            name = parseName(buffer, read_name - 1);
            printf("3\n");
            int result = addOnline(server_fd, name, read_name - 1, Accounts);
            if(result == 0){
                printf("4.0\n");
                write(server_fd, "ERROR", 5);
            } else{
                printf("4.1\n");
                write(server_fd, "SUCCESS", 7);
            }
            printf("5\n");
            // start communicating on a new thread.
            communicating(server_fd, Accounts);

        }
    }
    close(server_fd);
    close(sd);
}