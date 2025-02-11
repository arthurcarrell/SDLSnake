/*

Game engine that uses pixels

*/
#include <cstdio>
#include <cstring>
#define SDL_MAIN_USE_CALLBACKS 1 /* run SDL_AppInit instead of main() */

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_scancode.h>
#include <cstddef>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include "ac_engine/utils.hpp"
#include <iostream>


using namespace std;

bool debugMode = false;
const char PROJECT_Version[] = "1.0"; // project version; self explanatory.
const char PROJECT_ProjectName[] = "com.asteristired.snake"; // the internal name of the project, no spaces or special characters.
const char PROJECT_AppName[] = "Snake"; // the name that appears on the window.

const int WINDOW_Width = 720;
const int WINDOW_Height = 720;

const int FIXED_UPDATE_INTERVAL = 250; // ms

/* how many spaces there are in the game. DOES NOT represent the size of the window */
const int GAME_MAX_X = 24; 
const int GAME_MAX_Y = 24;

const int PIXELS_PER_CELL = 30; // if this number goes below 1, stuff breaks real bad. also this number MUST be cleanly divisible by (WINDOW_Width * WINDOW_Height)

int gameMap[GAME_MAX_X*GAME_MAX_Y];
/*
0 on the game map represents empty space
1 on the game map represents the snake body
2 on the game map represents the apple
3 on the game map represents the snake head
4 on the game map represents the dead snake
*/


static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

class Snake {
    public:
        vec2 position = {(GAME_MAX_X/2), (GAME_MAX_Y/2)};
        vector<vec2> previous_positions;
        int size = 0;
        char currentDirection = 'r';
        bool doesNeedToMove = false;

        // Snake.Move() cannot be ran in an SDL_Timer function, as it is a seperate thread. so this is a flag telling the main thread to run Snake.Move();
        bool isAlive = true;

        void Move(char direction) {
            // u,d,l,r 
            previous_positions.insert(previous_positions.begin(), position);
            if (direction == 'u') { position.y -= 1;}
            else if (direction == 'd') {position.y += 1;}
            else if (direction == 'l') {position.x -= 1;}
            else if (direction == 'r') {position.x += 1;}

            // check if the size of previous_positions is more then the snake size.
            if (previous_positions.size() > size) {
                // it is, so remove the last element.
                previous_positions.pop_back();
            }
        }
};

class Apple {
    public:
        vec2 position;

        void MoveRandom() {
            position.x = rand() % GAME_MAX_X;
            position.y = rand() % GAME_MAX_Y;
        }
};


Snake snake;
Apple apple;

/* SNAKE COLORS */
const color SNAKE_BODY_COLOR = {74, 208, 26, SDL_ALPHA_OPAQUE};
const color SNAKE_HEAD_COLOR = {51, 124, 24, SDL_ALPHA_OPAQUE};
const color DEAD_SNAKE_COLOR = {50,50,50, SDL_ALPHA_OPAQUE};

/* OTHER COLORS */
const color APPLE_COLOR = {246, 64, 83, SDL_ALPHA_OPAQUE};
const color BACKGROUND_COLOR = {0, 0, 0, SDL_ALPHA_OPAQUE};
/*
===========================================================================================================
                                                RENDERING CODE
===========================================================================================================
*/

/* Runs on program start */
void SnakeGame_Init(void **appstate, int argc, char *argv[]) {
    apple.MoveRandom();
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
    } else if (gameMap[currentCell] == 3){
        // the snake head, draw the snake head
        SDL_SetRenderDrawColor(renderer, SNAKE_HEAD_COLOR.R, SNAKE_HEAD_COLOR.G, SNAKE_HEAD_COLOR.B, SNAKE_HEAD_COLOR.A);
    } else if (gameMap[currentCell] == 4) {
        // the dead snake, draw the gray snake.
        SDL_SetRenderDrawColor(renderer, DEAD_SNAKE_COLOR.R, DEAD_SNAKE_COLOR.G, DEAD_SNAKE_COLOR.B, DEAD_SNAKE_COLOR.A);
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
    return coords.x + (coords.y * (x_max));
}

void DrawSnakeBody(Snake *snake) {
    for (long unsigned int i=0; i < snake->previous_positions.size();i++) {
        // snake[i] == current vec2 of the snake body
        int currentSnakeBodyPos = GetIndexOfCoords(snake->previous_positions[i], GAME_MAX_X);
        if (snake->isAlive) { gameMap[currentSnakeBodyPos] = 1; } else {gameMap[currentSnakeBodyPos] = 4; }
    }
}

void SnakeGame_DoKeyboardInput(const SDL_Event *event, Snake *snake) {
    // SDL_Event doesnt actually need to be a constant, its just incredibly bad practise to rewrite the event.

    // check that it is only a SDL_EVENT_KEY_DOWN function. otherwise there is an assertion failure.
    if (event->type != SDL_EVENT_KEY_DOWN) { return; }

    SDL_assert(event->type == SDL_EVENT_KEY_DOWN);

    // get the key press and see if its an arrow key, if it is, change the snake direction to match.
    if (event->key.scancode == SDL_SCANCODE_UP) {
        snake->currentDirection = 'u';
        
    } else if (event->key.scancode == SDL_SCANCODE_DOWN) {
        snake->currentDirection = 'd';

    } else if (event->key.scancode == SDL_SCANCODE_LEFT) {
        snake->currentDirection = 'l';

    } else if (event->key.scancode == SDL_SCANCODE_RIGHT) {
        snake->currentDirection = 'r';

    }
}

void GameOver(Snake *snake) {
    snake->isAlive = false;
}

Uint32 SnakeGame_FixedUpdate(void *userdata, Uint32 interval, Uint32 timestamp)  {
    // updates once per second.
    /* tell the snake to move, because this is asynchronous, we might be modifying the snake while the main loop is reading from it.
       if this happens, there will be a crash. So a flag is set to let the main loop know to move the snake when it can instead. */
    snake.doesNeedToMove = true;

    return FIXED_UPDATE_INTERVAL;
}

void doAppleCollision(Snake *snake, Apple *apple) {
    // are we colliding with the apple?
    if ((snake->position.x == apple->position.x) && (snake->position.y == apple->position.y)) {
        // we are colliding with the apple, increase the snake size by one, and move the apple randomly
        snake->size += 1;
        apple->MoveRandom();
    }
}

void doSnakeCollision(Snake *snake) {
    // check if we are colliding with ourselves.
    // this is done by checking if snake.previous_positions contains snake.position
    bool hasCollided = false;
    for (int i=0; i < snake->previous_positions.size();i++) {
        if ((snake->previous_positions.at(i).x == snake->position.x) && (snake->previous_positions.at(i).y == snake->position.y)) {
            // yes there is a collision
            hasCollided = true;
            break;
        }
    }

    if (hasCollided) {
        GameOver(snake);
    }
}

void doWallCollision(Snake *snake) {
    // LMAO, turns out the previous problem wasnt actually true, and it wasnt working because I didnt call this function :P

    if (snake->position.x > GAME_MAX_X-1) {
        // move the X position one back in order to prevent the loop around, and make it look like youve crashed into the wall.
        snake->position.x--;
        GameOver(snake);
    
    } else if (snake->position.x < 0) {
        // move the X position forward for the same reason described above.
        snake->position.x++;
        GameOver(snake);

    } else if (snake->position.y > GAME_MAX_Y-1) {
        // the Y position doesnt have to deal with the wrapping, but we will still adjust to get the snake on the screen.
        snake->position.y--;
        GameOver(snake);
        
    } else if (snake->position.y < 0) {
        // same as above
        snake->position.y++;
        GameOver(snake);
    }
}

void Game() {
    // reset the game map
    for (int i=0; i < (GAME_MAX_X*GAME_MAX_Y);i++) {
        gameMap[i] = 0;
    }

    // move snake if needed
    if (snake.doesNeedToMove && snake.isAlive) {
        snake.Move(snake.currentDirection);
        if (debugMode) { SDL_Log("X: %i - Y: %i - currentDirection: %c", snake.position.x, snake.position.y, snake.currentDirection); }
        snake.doesNeedToMove = false;
    }

    // place snake head
    int snakeIndex = GetIndexOfCoords(snake.position, GAME_MAX_X);
    if (snake.isAlive) { gameMap[snakeIndex] = 3; } else { gameMap[snakeIndex] = 4;}

    // place body
    DrawSnakeBody(&snake);

    // place apple
    gameMap[GetIndexOfCoords(apple.position, GAME_MAX_X)] = 2;


    // check if touching the apple.
    doAppleCollision(&snake, &apple);

    // check if snake head touching snake body
    doSnakeCollision(&snake);

    // do the wall collision
    doWallCollision(&snake);
    
}

/* Runs on key press/mouse click */
void SnakeGame_AppEvent(void *appstate, SDL_Event *event){
    SnakeGame_DoKeyboardInput(event, &snake);
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

    // initiate a random seed
    srand(time(NULL));

    // begin doing the timer for the fixed updates.
    SDL_AddTimer(FIXED_UPDATE_INTERVAL, SnakeGame_FixedUpdate, NULL);

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