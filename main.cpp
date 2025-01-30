/*

Game engine that uses pixels

*/
#define SDL_MAIN_USE_CALLBACKS 1 /* run SDL_AppInit instead of main() */

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <list>

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include "ac_engine/utils.hpp"
#include <iostream>


using namespace std;


const char PROJECT_Version[] = "1.0"; // project version; self explanatory.
const char PROJECT_ProjectName[] = "com.asteristired.snake"; // the internal name of the project, no spaces or special characters.
const char PROJECT_AppName[] = "Snake"; // the name that appears on the window.

const int WINDOW_Width = 1280;
const int WINDOW_Height = 720;

/* how many spaces there are in the game. DOES NOT represent the size of the window */
const int GAME_MAX_X = 100; 
const int GAME_MAX_Y = 100;

const int PIXELS_PER_CELL = 1; // if this number goes below 1, stuff breaks real bad.

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

class Snake {
    public:
        vec2 position;
        list<vec2> previous_positions;

};

class Apple {
    public:
        vec2 position;
};

Snake createSnake() {
    Snake snake;
    snake.position = {0, 0};
    return snake;
}

Snake snake = createSnake();
Apple apple;

const color SNAKE_BODY = {74, 208, 26, SDL_ALPHA_OPAQUE};
const color BACKGROUND_COLOR = {0, 0, 0, SDL_ALPHA_OPAQUE};
/*
===========================================================================================================
                                                SNAKE GAME
===========================================================================================================
*/

/* Runs on program start */
void SnakeGame_Init(void **appstate, int argc, char *argv[]) {
    return;
}

/* Runs on key press/mouse click */
void SnakeGame_AppEvent(void *appstate, SDL_Event *event) {
    return;
}

/* Runs each frame */
void SnakeGame_Iterate(void *appstate) {
    // set the background color to BACKGROUND and then clear the render.
    SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.R, BACKGROUND_COLOR.G, BACKGROUND_COLOR.B, BACKGROUND_COLOR.A);
    SDL_RenderClear(renderer);




    /* place the new render onto the screen */
    SDL_RenderPresent(renderer);
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
    SnakeGame_Iterate(appstate);
    return SDL_APP_CONTINUE;

}




/* Runs on shutdown */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL cleans up window/renderer */
}

int main() {
    cout << "Hello World!";
    return 1;
}