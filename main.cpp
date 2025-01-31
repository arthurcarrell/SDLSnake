/*

Game engine that uses pixels

*/
#define SDL_MAIN_USE_CALLBACKS 1 /* run SDL_AppInit instead of main() */

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <list>

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include "ac_engine/utils.hpp"
#include <iostream>


using namespace std;

bool debugMode = true;
const char PROJECT_Version[] = "1.0"; // project version; self explanatory.
const char PROJECT_ProjectName[] = "com.asteristired.snake"; // the internal name of the project, no spaces or special characters.
const char PROJECT_AppName[] = "Snake"; // the name that appears on the window.

const int WINDOW_Width = 720;
const int WINDOW_Height = 720;

/* how many spaces there are in the game. DOES NOT represent the size of the window */
const int GAME_MAX_X = 24; 
const int GAME_MAX_Y = 24;

const int PIXELS_PER_CELL = 30; // if this number goes below 1, stuff breaks real bad. also this number MUST be cleanly divisible by (WINDOW_Width * WINDOW_Height)

int gameMap[GAME_MAX_X*GAME_MAX_Y];
/*
0 on the game map represents empty space.
1 on the game map represents the snake body
2 on the game map represents the apple
*/


static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

class Snake {
    public:
        vec2 position;
        list<vec2> previous_positions;
        int size;

};

class Apple {
    public:
        vec2 position;
};

Snake createSnake() {
    Snake snake;
    snake.position = {11, 11};
    snake.size = 1;
    return snake;
}

Snake snake = createSnake();
Apple apple;

const color SNAKE_BODY_COLOR = {74, 208, 26, SDL_ALPHA_OPAQUE};
const color APPLE_COLOR = {246, 64, 83, SDL_ALPHA_OPAQUE};
const color BACKGROUND_COLOR = {0, 0, 0, SDL_ALPHA_OPAQUE};
/*
===========================================================================================================
                                                RENDERING CODE
===========================================================================================================
*/

/* Runs on program start */
void SnakeGame_Init(void **appstate, int argc, char *argv[]) {
    return;
}


int DrawCell(int x, int y) {
    SDL_FRect rectangle;
    rectangle.x = x;
    rectangle.y = y;
    rectangle.h = PIXELS_PER_CELL;
    rectangle.w = PIXELS_PER_CELL;

    if (debugMode) { SDL_RenderRect(renderer, &rectangle); } else {SDL_RenderFillRect(renderer, &rectangle); }
    // return 0 for success.
    return 0;
}

void HandleDrawData(int currentCell) {
    if (gameMap[currentCell] == 0) {
        // nothing, draw in the background color.
        if (debugMode) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);
        } else {
            SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.R, BACKGROUND_COLOR.G, BACKGROUND_COLOR.B, BACKGROUND_COLOR.A);
        }
        
    } else if (gameMap[currentCell] == 1) {
        // the snake body, draw the snake
        SDL_SetRenderDrawColor(renderer, SNAKE_BODY_COLOR.R, SNAKE_BODY_COLOR.G, SNAKE_BODY_COLOR.B, SNAKE_BODY_COLOR.A);
    } else if (gameMap[currentCell] == 2){
        // the apple, draw the apple.
        SDL_SetRenderDrawColor(renderer, APPLE_COLOR.R, APPLE_COLOR.G, APPLE_COLOR.B, APPLE_COLOR.A);
    }
}

void DrawCellMap() {
    int currentCell = 0;
    for (int y=0;y < GAME_MAX_Y; y++) {
        for (int x=0;x < GAME_MAX_X; x++) {
            HandleDrawData(currentCell);
            DrawCell(x*PIXELS_PER_CELL, y*PIXELS_PER_CELL);
            currentCell++;
        }
        currentCell++;
    }
}
/* Runs each frame */
void SnakeGame_Iterate(void *appstate) {
    // set the background color to BACKGROUND and then clear the render.
    SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.R, BACKGROUND_COLOR.G, BACKGROUND_COLOR.B, BACKGROUND_COLOR.A);
    SDL_RenderClear(renderer);
    
    DrawCellMap();
    /* place the new render onto the screen */
    SDL_RenderPresent(renderer);
    return;
}

/*
===========================================================================================================
                                                SNAKE GAME CODE
===========================================================================================================
*/

/*
our gameMap is stored as an array, meaning that there arent any coordinates. this takes the index in as input,
and returns an output as if it were a coordinate.
Example: (in a 10x10 array) foo[50] --> {x: 5, y: 5}
*/
vec2 GetCellCoords(int target,int x_max) {
    int x = target % x_max; // (target % width)
    int y = target / x_max;

    vec2 result = {x, y};
    return result;
}

/* 
    x + (y * (maxwidth_x+1))
    returns the index in a 1D array, of what the coordinates would be.
*/
int GetIndexOfCoords(vec2 coords, int x_max) {
    return coords.x + (coords.y * (x_max+1));
}

void Game() {
    // reset the game map
    for (int i=0; i < (GAME_MAX_X*GAME_MAX_Y)-1;i++) {
        gameMap[i] = 0;
    }

    // place snake head
    int snakeIndex = GetIndexOfCoords(snake.position, GAME_MAX_X);
    gameMap[snakeIndex] = 1;

    //debug. place apple in X: 2, Y: 2
    vec2 testCoord = {2,2};
    gameMap[GetIndexOfCoords(testCoord, GAME_MAX_X)] = 2;
    
}
/* Runs on key press/mouse click */
void SnakeGame_AppEvent(void *appstate, SDL_Event *event){
    return;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Runs on Startup */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

    /*
    Effectively just set everything up,
    if any part fails to run, return SDL_APP_FAILURE.
    if not, continue the script, and run SDL_APP_CONTINUE.
    */

    SDL_SetAppMetadata(PROJECT_AppName,PROJECT_Version,PROJECT_ProjectName);

    // init video
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        // something has broke.
        SDL_Log("Couldn't Initalise SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // create the window and the renderer
    if (!SDL_CreateWindowAndRenderer(PROJECT_AppName, WINDOW_Width, WINDOW_Height, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;

    }

    SnakeGame_Init(appstate,argc,argv);
    // everything has been properly intialised. continue with the program.
    return SDL_APP_CONTINUE;
}

/* runs on I/O interrupt. This could be a keypress or a mouse click. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    SnakeGame_AppEvent(appstate, event);
    if (event->type == SDL_EVENT_QUIT) {
        // a key that means quit the program has been pressed. (my example: SUPER+SHIFT+Q)
        return SDL_APP_SUCCESS; // quit the program, but in an intentional way, we are not quitting due to an error.
    }

    return SDL_APP_CONTINUE; // key was not a quit key, ignore and continue.
}

/* Runs once per frame, the meat and potatoes of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    // success, continue the program.
    Game();
    SnakeGame_Iterate(appstate);
    return SDL_APP_CONTINUE;

}




/* Runs on shutdown */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL cleans up window/renderer */
}

int main() {
    cout << "Hello World! if you are seeing this, something went wrong.";
    return 1;
}