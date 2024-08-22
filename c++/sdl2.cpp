#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]){

  SDL_Init(SDL_INIT_VIDEO);   // Initialize SDL2

  SDL_Window *window;        // Declare a pointer to an SDL_Window

  // Create an application window with the following settings:
  window = SDL_CreateWindow( 
    "An SDL2 window",                  //    window title
    SDL_WINDOWPOS_UNDEFINED,           //    initial x position
    SDL_WINDOWPOS_UNDEFINED,           //    initial y position
    640,                               //    width, in pixels
    480,                               //    height, in pixels
    SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL //    flags - see below
  );

  // Check that the window was successfully made
  if(window==NULL){   
    // In the event that the window could not be made...
    std::cout << "Could not create window: " << SDL_GetError() << '\n';
    return 1;
  }

  // The window is open: enter program loop (see SDL_PollEvent)

  SDL_Delay(3000);  // Pause execution for 3000 milliseconds, for example

  // Close and destroy the window
  SDL_DestroyWindow(window); 

  // Clean up
  SDL_Quit(); 
  return 0;   

}
