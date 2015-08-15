#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
int counter = 0;
char textBuffer[256];


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


void printInputString(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect *destRect, SDL_Rect *srcRect){


    SDL_Color color = {0,255,0}; // font color
    SDL_Color bgcolor = {128,128,128}; // background font color, used only in Shaded

    printf("glyph = %s, counter = %d\n", textBuffer, counter);
                

    SDL_Surface *textSurface;
    SDL_Texture *textTexture;
    textSurface = TTF_RenderUTF8_Blended(font, textBuffer, color);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_RenderCopy(renderer, textTexture, srcRect, destRect);
    SDL_RenderPresent(renderer);

    int advance;
    TTF_GlyphMetrics(font, textBuffer[counter - 1], NULL, NULL, NULL, NULL, &advance);
    assert(TTF_GlyphIsProvided(font, textBuffer[counter - 1])); // check that a glyph exists, change to different?    

    srcRect->h = destRect->h; // multiple rows?
    destRect->w += advance; // updates width
    srcRect->w = destRect->w;

#if 0
    SDL_Event event;
    while(typing){
        SDL_WaitEvent(&event); // blocking waiting for event
        switch(event.type){
            
            case SDL_QUIT:

                typing = 0;
                exit(0);
            break;
            default:
            printf("type = %d\n", event.type);
            break;
        }
        event.type = 0;
        
    }
    TTF_CloseFont(font);
#endif
    SDL_StopTextInput();
    return;
}

void renderGUI(SDL_Renderer *renderer, int x, int y, int w, int h, int r, int g, int b){
    SDL_Rect srcRect;
    SDL_Rect destRect;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = w;
    srcRect.h = h;

    destRect.x = x;
    destRect.y = y;
    destRect.w = w;
    destRect.h = h;

    SDL_Surface *surfaceBox;
    SDL_Texture *texTexture;
    if((surfaceBox = SDL_CreateRGBSurface(0, 10, 10, 32, 0, 0, 0, 0)) == NULL){
        printf("ERROR: %s\n", SDL_GetError());
    }
    SDL_FillRect(surfaceBox, &srcRect, SDL_MapRGB(surfaceBox->format, r, g, b));
    texTexture = SDL_CreateTextureFromSurface(renderer, surfaceBox);
    SDL_FreeSurface(surfaceBox);
    SDL_RenderCopy(renderer, texTexture, &srcRect, &destRect);
    SDL_DestroyTexture(texTexture);


}

int main(int argc, char* argv[]){

    ///////////
    //  GUI  //
    ///////////

    // Create window
    SDL_Window *window;
    //SDL_Surface *screen;
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("Den ekologiska chatten", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    //screen = SDL_GetWindowSurface(window);
    //SDL_Surface* screenSurface;
    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);


    TTF_Font *font;
    if((font = TTF_OpenFont("fonts/FreeSerif.ttf", 20)) == NULL){
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
    int fontAscent = TTF_FontAscent(font);
    int fontDescent = TTF_FontDescent(font);

  

    SDL_Rect srcRect;
    SDL_Rect destRect;
    int baseline = SCREEN_HEIGHT - 20;

    srcRect.x = 0;
    srcRect.y = 0;

    destRect.x = SCREEN_WIDTH / 2 - 90;
    destRect.y = baseline - fontAscent;
    destRect.w = 0; //start with nothing and expand when you type. Will lose everything if restarted.
    destRect.h = fontAscent - fontDescent;
    srcRect.h = destRect.h; // multiple rows?


    SDL_SetTextInputRect(&srcRect);
    SDL_StartTextInput();
    int messageInput = 0;
    char *composition;
    int cursor;
    int selection_len;

    // Update the window
    int running = 1;
    while(running){
        SDL_RenderClear(renderer);
        renderGUI(renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 255, 255, 255); // background
        renderGUI(renderer, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 40, SCREEN_WIDTH / 2 + 90, 30, 200, 200, 200); // textinput
        renderGUI(renderer, SCREEN_WIDTH / 2 - 100, 10, SCREEN_WIDTH / 2 + 90, SCREEN_HEIGHT - 60, 200, 200, 200); //recieve text box
        renderGUI(renderer, 10, 10, SCREEN_WIDTH / 2 - 120, 100, 200, 200, 200); // profile box
        renderGUI(renderer, 10, 120, SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT - 130, 200, 200, 200); // friends list
        
        if (counter > 0){
            printInputString(renderer, font, &destRect, &srcRect);
        }

        SDL_RenderPresent( renderer );
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch(event.type){
            
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT){
                    printf("type: %d, BUTTON_LEFT: %d\n", event.button.button, SDL_BUTTON_LEFT);
                    if((event.button.x > SCREEN_WIDTH / 2 - 100) && 
                        (event.button.x < SCREEN_WIDTH - 10) &&
                        (event.button.y > SCREEN_HEIGHT - 40) && 
                        (event.button.y < SCREEN_HEIGHT - 10)){
                        // Text Input
                        messageInput = 1;
                        printf("Input activated\n");
                    }
                    else {
                        messageInput = 0;
                    }
                }
            break;
            case SDL_KEYDOWN:
                if((event.key.keysym.sym == SDLK_BACKSPACE) && messageInput){
                    //handle backspace
                    if(counter > 0){
                        int advance;
                        TTF_GlyphMetrics(font, textBuffer[counter], NULL, NULL, NULL, NULL, &advance);
                        destRect.w -= advance;
                        srcRect.w = destRect.w;
                        textBuffer[counter] = '\0';
                        --counter;
                    }
                    break;
                } else if(event.key.keysym.sym == SDLK_RETURN && messageInput){
                    //handle retrun
                    if(counter > 0){
                        //print message to message box
                        counter = 0;
                        destRect.w = 0;
                        srcRect.w = 0;
                        textBuffer[0] = '\0';
                    }
                    break;
                }
            break;
            case SDL_TEXTINPUT: // writing text
                if(messageInput){
                    printf("%s\n", event.text.text);
                    printf("counter = %d\n", counter);
                    textBuffer[counter] = *event.text.text;
                    ++counter; // counter declared at top
                }
            break;
            case SDL_QUIT:
                running = 0;
            break;
        }
        event.type = 0;
        //sleep(0.1);
    
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

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