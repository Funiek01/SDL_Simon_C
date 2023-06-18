#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define FPS 30
#define WINDOWWIDTH 640
#define WINDOWHEIGHT 480
#define FLASHSPEED 1000  // in milliseconds
#define FLASHDELAY 400  // in milliseconds
#define BUTTONSIZE 200
#define BUTTONGAPSIZE 20

//      R    G    B       colors
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define BRIGHTRED 0xFF0000
#define BRIGHTGREEN 0x00FF00
#define BRIGHTBLUE 0x0000FF
#define BRIGHTYELLOW 0xFFFF00


Uint32 bgColor = BLACK;


int XMARGIN = (WINDOWWIDTH - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2;
int YMARGIN = (WINDOWHEIGHT - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2;


Mix_Chunk* SOUND1 = NULL;
Mix_Chunk* SOUND2 = NULL;
Mix_Chunk* SOUND3 = NULL;
Mix_Chunk* SOUND4 = NULL;
Mix_Chunk* SOUND5 = NULL;
Mix_Chunk* SOUND6 = NULL;

// SDL_Rect objects for each of the four buttons
SDL_Rect YELLOWRECT = { (WINDOWWIDTH - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2, (WINDOWHEIGHT - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2 , BUTTONSIZE, BUTTONSIZE };
SDL_Rect BLUERECT = { (WINDOWWIDTH - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2 + BUTTONSIZE + BUTTONGAPSIZE, (WINDOWHEIGHT - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2 , BUTTONSIZE, BUTTONSIZE };
SDL_Rect REDRECT = { (WINDOWWIDTH - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2, (WINDOWHEIGHT - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2 + BUTTONSIZE + BUTTONGAPSIZE, BUTTONSIZE, BUTTONSIZE };
SDL_Rect GREENRECT = { (WINDOWWIDTH - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2 + BUTTONSIZE + BUTTONGAPSIZE, (WINDOWHEIGHT - (2 * BUTTONSIZE) - BUTTONGAPSIZE) / 2 + BUTTONSIZE + BUTTONGAPSIZE, BUTTONSIZE, BUTTONSIZE };


// Enum for the different button colors
enum button_color {
    YELLOW = 0x9B9B00,
    BLUE = 0x00009B,
    RED = 0x9B0000,
    GREEN = 0x009B00
};
typedef enum button_color button_color;

enum button_color pattern[10];  // stores the pattern of colors
typedef int colorsHandle[100];
int counter = 0;
int globalvar = 0;
int newhandler[] = {YELLOW, BLUE,RED,GREEN};
int currentStep = 0;
int score = 0;
int randombutton = 0;
bool waitingForInput = false;  // when false, the pattern is playing. when true, waiting for the player to click a colored button
bool flag = false;
SDL_Renderer* renderer = NULL;
button_color clickedButton = -1;
button_color colorsNew[10];

void drawButtons() {
    SDL_SetRenderDrawColor(renderer, YELLOW >> 16, (YELLOW >> 8) & 0xff, YELLOW & 0xff, 0xff);
    SDL_RenderFillRect(renderer, &YELLOWRECT);
    SDL_SetRenderDrawColor(renderer, BLUE >> 16, (BLUE >> 8) & 0xff, BLUE & 0xff, 0xff);
    SDL_RenderFillRect(renderer, &BLUERECT);
    SDL_SetRenderDrawColor(renderer, RED >> 16, (RED >> 8) & 0xff, RED & 0xff, 0xff);
    SDL_RenderFillRect(renderer, &REDRECT);
    SDL_SetRenderDrawColor(renderer, GREEN >> 16, (GREEN >> 8) & 0xff, GREEN & 0xff, 0xff);
    SDL_RenderFillRect(renderer, &GREENRECT);

    //update the screen with the new button colors
    SDL_RenderPresent(renderer);
}


void flashButton(button_color color) {
    // Code to flash the button of the given color
    switch (color) {
    case YELLOW:
        SDL_SetRenderDrawColor(renderer, BRIGHTYELLOW >> 16, (BRIGHTYELLOW >> 8) & 0xff, BRIGHTYELLOW & 0xff, 0xff);
        SDL_RenderFillRect(renderer, &YELLOWRECT);
        Mix_PlayChannel(-1, SOUND1, 0);
        break;
    case BLUE:
        SDL_SetRenderDrawColor(renderer, BRIGHTBLUE >> 16, (BRIGHTBLUE >> 8) & 0xff, BRIGHTBLUE & 0xff, 0xff);
        SDL_RenderFillRect(renderer, &BLUERECT);
        Mix_PlayChannel(-1, SOUND2, 0);
        break;
    case RED:
        SDL_SetRenderDrawColor(renderer, BRIGHTRED >> 16, (BRIGHTRED >> 8) & 0xff, BRIGHTRED & 0xff, 0xff);
        SDL_RenderFillRect(renderer, &REDRECT);
        Mix_PlayChannel(-1, SOUND3, 0);
        break;
    case GREEN:
        SDL_SetRenderDrawColor(renderer, BRIGHTGREEN >> 16, (BRIGHTGREEN >> 8) & 0xff, BRIGHTGREEN & 0xff, 0xff);
        SDL_RenderFillRect(renderer, &GREENRECT);
        Mix_PlayChannel(-1, SOUND4, 0);
        break;
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(FLASHDELAY);
    drawButtons();
}


void handleButtonClick(int x, int y) {
    waitingForInput = true;
    // Code to handle button click event
    button_color clickedButton = -1;
    if (YELLOWRECT.x < x && x < YELLOWRECT.x + BUTTONSIZE && YELLOWRECT.y < y && y < YELLOWRECT.y + BUTTONSIZE) {
        clickedButton = YELLOW;
    }
    else if (BLUERECT.x < x && x < BLUERECT.x + BUTTONSIZE && BLUERECT.y < y && y < BLUERECT.y + BUTTONSIZE) {
        clickedButton = BLUE;
    }
    else if (REDRECT.x < x && x < REDRECT.x + BUTTONSIZE && REDRECT.y < y && y < REDRECT.y + BUTTONSIZE) {
        clickedButton = RED;
    }
    else if (GREENRECT.x < x && x < GREENRECT.x + BUTTONSIZE && GREENRECT.y < y && y < GREENRECT.y + BUTTONSIZE) {
        clickedButton = GREEN;
    }
    flashButton(clickedButton);
    randombutton = clickedButton;
    return randombutton;
}

// function to handle mouse events
void handleMouseEvent(SDL_Event event) {
    int x = event.button.x;
    int y = event.button.y;
    handleButtonClick(x, y);
    return randombutton;
}

void drawScore() {
    // Clear the score area
    SDL_Rect scoreRect = { 0, 0, WINDOWWIDTH, 10 };
    SDL_SetRenderDrawColor(renderer, bgColor >> 16, (bgColor >> 8) & 0xff, bgColor & 0xff, 0xff);
    SDL_RenderFillRect(renderer, &scoreRect);

    // Draw the score
    TTF_Font* font = TTF_OpenFont("/home/funiek/Desktop/new_project/raleway/Raleway-Black.ttf", 20);
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = { 5, 5, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}



void displayGameOver() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, bgColor >> 16, (bgColor >> 8) & 0xff, bgColor & 0xff, 0xff);
    SDL_RenderClear(renderer);

    // Draw game over message
    TTF_Font* font = TTF_OpenFont("/home/funiek/Desktop/new_project/raleway/Raleway-Black.ttf", 48);
    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Game Over!", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = { (WINDOWWIDTH - surface->w) / 2, (WINDOWHEIGHT - surface->h) / 2, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_RenderPresent(renderer);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);

    // Wait for a key press
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                done = true;
            }
        }
    }
}

void gameOver() {
    Mix_PlayChannel(-1, SOUND5, 0);
    displayGameOver();
    SDL_Delay(3000);
    Mix_HaltMusic();
    Mix_FreeChunk(SOUND1);
    Mix_FreeChunk(SOUND2);
    Mix_FreeChunk(SOUND3);
    Mix_FreeChunk(SOUND4);
    Mix_FreeChunk(SOUND5);
    Mix_FreeChunk(SOUND6);
    Mix_CloseAudio();
    SDL_Quit();
    exit(0);
}

bool displayHelp(SDL_Renderer* renderer) {
    bool helpScreenActive = true;
    while (helpScreenActive) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                helpScreenActive = false;
                break;
            }
        }

	// Clear the screen
	SDL_SetRenderDrawColor(renderer, bgColor >> 16, (bgColor >> 8) & 0xff, bgColor & 0xff, 0xff);
	SDL_RenderClear(renderer);

	// Draw game over message
	TTF_Font* font = TTF_OpenFont("/home/funiek/Desktop/new_project/raleway/Raleway-Black.ttf", 18);
	// Draw game over message

	SDL_Color textColor = { 255, 255, 255 };
	SDL_Surface* line1Surface = TTF_RenderText_Solid(font, "1. Press button 'Play' to start the game.", textColor);
	SDL_Texture* line1Texture = SDL_CreateTextureFromSurface(renderer, line1Surface);
	SDL_Rect line1Rect = { XMARGIN-80, YMARGIN, line1Surface->w, line1Surface->h };

	SDL_Surface* line2Surface = TTF_RenderText_Solid(font, "2.Game will generate a random pattern of colors and show it by flash.", textColor);
	SDL_Texture* line2Texture = SDL_CreateTextureFromSurface(renderer, line2Surface);
	SDL_Rect line2Rect = { XMARGIN-80, YMARGIN + line1Surface->h, line2Surface->w, line2Surface->h };

	SDL_Surface* line3Surface = TTF_RenderText_Solid(font, "3. Click with the left mouse button on the correct color button.", textColor);
	SDL_Texture* line3Texture = SDL_CreateTextureFromSurface(renderer, line3Surface);
	SDL_Rect line3Rect = { XMARGIN-80, YMARGIN + line1Surface->h + line2Surface->h, line3Surface->w, line3Surface->h };

	SDL_Surface* line4Surface = TTF_RenderText_Solid(font, "4. If you press all buttons correctly, you will get a point (score).", textColor);
	SDL_Texture* line4Texture = SDL_CreateTextureFromSurface(renderer, line4Surface);
	SDL_Rect line4Rect = { XMARGIN-80, YMARGIN + line1Surface->h + line2Surface->h + line3Surface->h, line4Surface->w, line4Surface->h };

	SDL_Surface* line5Surface = TTF_RenderText_Solid(font, "5. If you press wrong button, there will get notification 'GAMEOVER'", textColor);
	SDL_Texture* line5Texture = SDL_CreateTextureFromSurface(renderer, line5Surface);
	SDL_Rect line5Rect = { XMARGIN-80, YMARGIN + line1Surface->h + line2Surface->h + line3Surface->h + line4Surface->h, line5Surface->w, line5Surface->h };

    SDL_Surface* line6Surface = TTF_RenderText_Solid(font, "6.If you want to exit then click 'x' in right upper corner", textColor);
	SDL_Texture* line6Texture = SDL_CreateTextureFromSurface(renderer, line6Surface);
	SDL_Rect line6Rect = { XMARGIN-80, YMARGIN + line1Surface->h + line2Surface->h + line3Surface->h + line4Surface->h + line5Surface->h, line6Surface->w, line6Surface->h };

	SDL_Surface* line7Surface = TTF_RenderText_Solid(font, "7. Good luck!", textColor);
	SDL_Texture* line7Texture = SDL_CreateTextureFromSurface(renderer, line7Surface);
	SDL_Rect line7Rect = { XMARGIN-80, YMARGIN + line1Surface->h + line2Surface->h + line3Surface->h + line4Surface->h + line5Surface->h + line6Surface->h, line7Surface->w, line7Surface->h};

	SDL_RenderCopy(renderer, line1Texture, NULL, &line1Rect);
	SDL_FreeSurface(line1Surface);
	SDL_DestroyTexture(line1Texture);
    SDL_RenderCopy(renderer, line2Texture, NULL, &line2Rect);
	SDL_FreeSurface(line2Surface);
	SDL_DestroyTexture(line2Texture);
    SDL_RenderCopy(renderer, line3Texture, NULL, &line3Rect);
	SDL_FreeSurface(line3Surface);
	SDL_DestroyTexture(line3Texture);
    SDL_RenderCopy(renderer, line4Texture, NULL, &line4Rect);
	SDL_FreeSurface(line4Surface);
	SDL_DestroyTexture(line4Texture);
    SDL_RenderCopy(renderer, line5Texture, NULL, &line5Rect);
	SDL_FreeSurface(line5Surface);
	SDL_DestroyTexture(line5Texture);
    SDL_RenderCopy(renderer, line6Texture, NULL, &line6Rect);
	SDL_FreeSurface(line6Surface);
	SDL_DestroyTexture(line6Texture);
    SDL_RenderCopy(renderer, line7Texture, NULL, &line7Rect);
	SDL_FreeSurface(line7Surface);
	SDL_DestroyTexture(line7Texture);

	SDL_RenderPresent(renderer);
	TTF_CloseFont(font);

        SDL_RenderPresent(renderer);
    }
    return helpScreenActive;
}



void menu(SDL_Renderer* renderer) {
    bool menu = true;
    bool wait = false;
    TTF_Font* font = TTF_OpenFont("/home/funiek/Desktop/new_project/raleway/Raleway-Regular.ttf", 20);
    if (!font) {
    printf("TTF_OpenFont: %s\n", TTF_GetError());
    menu = false;
    }
    SDL_Color textColor = { 0, 255, 0 };
    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Play", textColor);
    SDL_Texture* playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_Rect playRect = { XMARGIN, YMARGIN, BUTTONSIZE, BUTTONSIZE };

    SDL_Surface* helpSurface = TTF_RenderText_Solid(font, "Help", textColor);
    SDL_Texture* helpTexture = SDL_CreateTextureFromSurface(renderer, helpSurface);
    SDL_Rect helpRect = { XMARGIN + BUTTONSIZE + BUTTONGAPSIZE, YMARGIN, BUTTONSIZE, BUTTONSIZE };
    Mix_PlayChannel(-1, SOUND6, 0);


while (menu) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            menu = false;
            break;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= playRect.x && x <= playRect.x + playRect.w && y >= playRect.y && y <= playRect.y + playRect.h) {
                Mix_FreeChunk(SOUND6);
                flag = true;
                menu = false;
                break;
            } else if (x >= helpRect.x && x <= helpRect.x + helpRect.w && y >= helpRect.y && y <= helpRect.y + helpRect.h) {
                if (!displayHelp(renderer)) {
                    // Help screen closed, return to menu
                    break;
                }
            }
        }
    }

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, bgColor >> 16, (bgColor >> 8) & 0xff, bgColor & 0xff, 0xff);
    SDL_RenderClear(renderer);

    // Draw the Play button
    SDL_RenderCopy(renderer, playTexture, NULL, &playRect);

    // Draw the Help button
    SDL_RenderCopy(renderer, helpTexture, NULL, &helpRect);

    // Update the screen
    SDL_RenderPresent(renderer);
}

// Clean up
SDL_DestroyTexture(playTexture);
SDL_FreeSurface(playSurface);
SDL_DestroyTexture(helpTexture);
SDL_FreeSurface(helpSurface);
TTF_CloseFont(font);
}



void startNewPattern() {
    for (int i = 0; i < currentStep+1; i++) {
        pattern[i] = (button_color)(rand() % 4);
        globalvar = newhandler[pattern[i]];
        colorsNew[i] = pattern[i];
        flashButton(globalvar);
        SDL_Delay(FLASHDELAY);
        counter++;
    }
}




int main(int argc, char* argv[]) {
    srand(time(NULL));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create the window and renderer
    SDL_Window* window = SDL_CreateWindow("Simon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOWWIDTH, WINDOWHEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error initializing SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    // Load sound effects
    SOUND1 = Mix_LoadWAV("samples/fa.wav");
    SOUND2 = Mix_LoadWAV("samples/mi.wav");
    SOUND3 = Mix_LoadWAV("samples/si.wav");
    SOUND4 = Mix_LoadWAV("samples/sol.wav");
    SOUND5 = Mix_LoadWAV("samples/gameover.wav");
    SOUND6 = Mix_LoadWAV("samples/ukulele.wav");

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        return 1;
    }
    menu(renderer);

    // Main game loop
    while (flag) {

        drawScore();
        // Handle events
        SDL_Event event;
        drawButtons();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                exit(0);
            }
            if (event.type == SDL_MOUSEBUTTONDOWN){
            handleMouseEvent(event);
            }
            if (waitingForInput == false){
                SDL_RenderPresent(renderer);
                SDL_Delay(1000);
                drawScore();
                startNewPattern();
                waitingForInput=true;
            }
            else{
                if (event.type == SDL_MOUSEBUTTONDOWN){
                //flashButton(clickedButton);

                button_color currentColor = -1;

                for (int j = 0; j < currentStep+1; j++) {
                    if (colorsNew[j] == 0){
                    currentColor = YELLOW;
                    }
                    else if (colorsNew[j] == 1){
                    currentColor = BLUE;
                    }
                    else if (colorsNew[j] == 2){
                    currentColor = RED;
                    }
                    else if (colorsNew[j] == 3){
                    currentColor = GREEN;
                    }
                    if (randombutton == currentColor){
                        currentStep++;
                        if(currentStep == counter){
                            score++;
                            waitingForInput = false;
                            currentStep = 0;
                            SDL_Delay(2000);
                            startNewPattern();
                        }
                        break;
                    }else{
                        gameOver();
                        printf("Game over!");
                    }
                }

                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, bgColor >> 16, (bgColor >> 8) & 0xff, bgColor & 0xff, 0xff);
        SDL_RenderClear(renderer);
        // Draw the buttons
        drawButtons();

        // Draw the score
        drawScore();

        // Update the screen
        SDL_RenderPresent(renderer);

        // Wait for a bit to slow down the game loop
        SDL_Delay(1000 / FPS);
    }

    Mix_FreeChunk(SOUND1);
    Mix_FreeChunk(SOUND2);
    Mix_FreeChunk(SOUND3);
    Mix_FreeChunk(SOUND4);
    Mix_FreeChunk(SOUND5);
    Mix_FreeChunk(SOUND6);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
