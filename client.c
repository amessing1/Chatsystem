#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <SDL2/SDL.h>



char * parseName(char buffer[], int length){
    char *name = malloc(length);
    int i;
    for(i = 0; i < length; i++){
        name[i] = buffer[i];
    }
    return name;
}

void *recieveMessages(void *args){
    char buffer[256];
    char name[11];
    char *message;
    char *from;
    int online = 1;
    long sd = (long)args;
    while(online){
        int bytes_read = read(sd, name, sizeof(name));
        from = parseName(name, bytes_read);
        bytes_read = read(sd, buffer, sizeof(buffer));
        printf("From %s: %s\n", from, buffer);
    }
}

int main(int argc, char* argv[]){

    ///////////
    //  GUI  //
    ///////////

    
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    // Create window
    SDL_Window *window;
    //SDL_Surface *screen;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chat program name", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    //screen = SDL_GetWindowSurface(window);
    //SDL_Surface* screenSurface;
    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);

    SDL_Rect srcRect;
    SDL_Rect destRect;

    // Text input box

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = SCREEN_WIDTH / 2;
    srcRect.h = 30;

    destRect.x = SCREEN_WIDTH / 2 - 100;
    destRect.y = SCREEN_HEIGHT - 40;
    destRect.w = SCREEN_WIDTH / 2 + 90;
    destRect.h = 30;

    SDL_Surface *textInputBackground;
    SDL_Texture *textInputTexture;
    if((textInputBackground = SDL_CreateRGBSurface(0, 10, 10, 32, 0, 0, 0, 0)) == NULL){
        printf("ERROR: %s\n", SDL_GetError());
    }
    SDL_FillRect(textInputBackground, &srcRect, SDL_MapRGB(textInputBackground->format, 255, 255, 0));
    textInputTexture = SDL_CreateTextureFromSurface(renderer, textInputBackground);
    SDL_FreeSurface(textInputBackground);
    SDL_RenderCopy(renderer, textInputTexture, &srcRect, &destRect);


    // Recieve Message box

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = SCREEN_WIDTH / 2;
    srcRect.h = SCREEN_HEIGHT - 60;

    destRect.x = SCREEN_WIDTH / 2 - 100;
    destRect.y = 10;
    destRect.w = SCREEN_WIDTH / 2 + 90;
    destRect.h = SCREEN_HEIGHT - 60;

    SDL_Surface *textRecieveBox;
    SDL_Texture *recieveBoxTexture;
    if((textRecieveBox = SDL_CreateRGBSurface(0, 10, 10, 32, 0, 0, 0, 0)) == NULL){
        printf("ERROR: %s\n", SDL_GetError());
    }
    SDL_FillRect(textRecieveBox, &srcRect, SDL_MapRGB(textRecieveBox->format, 255, 0, 0));
    recieveBoxTexture = SDL_CreateTextureFromSurface(renderer, textRecieveBox);
    SDL_FreeSurface(textRecieveBox);
    SDL_RenderCopy(renderer, recieveBoxTexture, &srcRect, &destRect);


    // Profile Box

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = SCREEN_WIDTH / 2 - 120;
    srcRect.h = 100;

    destRect.x = 10;
    destRect.y = 10;
    destRect.w = SCREEN_WIDTH / 2 - 120;
    destRect.h = 100;

    SDL_Surface *profileBox;
    SDL_Texture *profileBoxTexture;
    if((profileBox = SDL_CreateRGBSurface(0, 10, 10, 32, 0, 0, 0, 0)) == NULL){
        printf("ERROR: %s\n", SDL_GetError());
    }
    SDL_FillRect(profileBox, &srcRect, SDL_MapRGB(profileBox->format, 0, 0, 255));
    profileBoxTexture = SDL_CreateTextureFromSurface(renderer, profileBox);
    SDL_FreeSurface(profileBox);
    SDL_RenderCopy(renderer, profileBoxTexture, &srcRect, &destRect);


    // Firends list

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = SCREEN_WIDTH / 2 - 120;
    srcRect.h = SCREEN_HEIGHT - 130;

    destRect.x = 10;
    destRect.y = 120;
    destRect.w = SCREEN_WIDTH / 2 - 120;
    destRect.h = SCREEN_HEIGHT - 130;

    SDL_Surface *friendsBox;
    SDL_Texture *friendsBoxTexture;
    if((friendsBox = SDL_CreateRGBSurface(0, 10, 10, 32, 0, 0, 0, 0)) == NULL){
        printf("ERROR: %s\n", SDL_GetError());
    }
    SDL_FillRect(friendsBox, &srcRect, SDL_MapRGB(friendsBox->format, 0, 255, 255));
    friendsBoxTexture = SDL_CreateTextureFromSurface(renderer, friendsBox);
    SDL_FreeSurface(friendsBox);
    SDL_RenderCopy(renderer, friendsBoxTexture, &srcRect, &destRect);

    // Update the window
    while(1){
        SDL_RenderPresent( renderer );
        sleep(1);
    }


    /////////
#if 0
    char *Server_Address = "127.0.0.1";
    uint16_t port = atoi(argv[1]);
    char buffer[256];
    char name[11];
    int online = 1;
    char *your_name;
    pthread_t pid;

    long socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    printf("Hi, please enter your name:\n");
    int name_bytes = read(0, name, sizeof(name));
    your_name = parseName(name, name_bytes);
    if(name_bytes == 11){
        printf("Your name is too long. Pick another one(Max 10 characters):\n");
        //go to read^
    }
    printf("Telling server that im online\n");
    write(socket_descriptor, name, name_bytes); // Tell server you are online.
    printf("recieve confirmation\n");
    read(socket_descriptor, buffer, sizeof(buffer)); // recieve confirmation that your name is valid
    if(strcmp(buffer, "ERROR") == 0){
        printf("Name taken, pick a new one:\n");
        //go to read
    } else if(strcmp(buffer, "SUCCESS") == 0) {
        printf("SUCCESS\n");
    }
    printf("Welcome %s", your_name);
    printf("You can now start chatting\n");
    printf("-------\n");

    pthread_create(&pid, NULL, (void *)recieveMessages, (void *)socket_descriptor);

    while(online){
        printf("To:\n");
        name_bytes = read(0, name, sizeof(name));
        printf("write message:\n");
        int bytes_read = read(0, buffer, sizeof(buffer));
        if(strcmp(buffer, "exit") == 0){
            online = 0;
        }
        //write(1, buffer, bytes_read);
        write(socket_descriptor, name, name_bytes);
        write(socket_descriptor, buffer, bytes_read);
    }
    close(socket_descriptor);
#endif
    return 0;
}