/*

SDL3 First test.
This is the first time ive ever used SDL3. or really SDL in general unless you count one failed project.

This is the code that im """borrowing""" from.
https://examples.libsdl.org/SDL3/renderer/01-clear/

*/
#define SDL_MAIN_USE_CALLBACKS 1 /* run SDL_AppInit instead of main() */

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <iostream>


using namespace std;


const char PROJECT_Version[] = "1.0"; // project version; self explanatory.
const char PROJECT_ProjectName[] = "com.asteristired.sdl3_test"; // the internal name of the project, no spaces or special characters.
const char PROJECT_AppName[] = "SDL3 Test!"; // the name that appears on the window.

const int WINDOW_Width = 1280;
const int WINDOW_Height = 720;

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

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

    // everything has been properly intialised. continue with the program.
    return SDL_APP_CONTINUE;
}

/* runs on I/O interrupt. This could be a keypress or a mouse click. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        // a key that means quit the program has been pressed. (my example: SUPER+SHIFT+Q)
        return SDL_APP_SUCCESS; // quit the program, but in an intentional way, we are not quitting due to an error.
    }
    return SDL_APP_CONTINUE; // key was not a quit key, ignore and continue.
}



/* Runs once per frame, the meat and potatoes of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    const double now = ((double)SDL_GetTicks()) / 1000.0; // convert from miliseconds to seconds.
    /*
    A sine wave is used in order to smoothly flow through the colours.
    This uses floating points to represent colours, however im gonna change that at some point to use RGBA.
    */
    const float red = (float) (0.5 + 0.5 * SDL_sin(now));
    const float green = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
    const float blue = (float) (0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));

    // set the render draw colour to the floating point value, being fully opaque. 
    // TODO: Change this to use RGBA instead of floating points.
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);

    /* clear the window to the draw colour */
    SDL_RenderClear(renderer);
    
    /* place the new render onto the screen */
    SDL_RenderPresent(renderer);

    // success, continue the program.
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