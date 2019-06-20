#include "../screen.h"

namespace screen{

  SDL_Window* window;
  SDL_GLContext gl;

  float bounds[2];

  int si;

  int pixelBounds[2];

  void init(){

    SDL_Init(SDL_INIT_VIDEO);

    #ifdef __ANDROID__

    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0,0,&mode);
    pixelBounds[0] = mode.w;
    pixelBounds[1] = mode.h;

    #else

    pixelBounds[0] = 1024;//960;
    pixelBounds[1] = 1024;//540;

    #endif

    bounds[0] = 1;
    bounds[1] = float(pixelBounds[1])/pixelBounds[0];


    window = SDL_CreateWindow("Coventina",0,0,pixelBounds[0],pixelBounds[1],SDL_WINDOW_OPENGL /*| SDL_WINDOW_FULLSCREEN_DESKTOP */| SDL_WINDOW_RESIZABLE);
    SDL_GL_GetDrawableSize(window, &pixelBounds[0], &pixelBounds[1]);

    #ifdef __ANDROID__
    SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN);
    #endif

    gl = SDL_GL_CreateContext(window);

    if(bounds[0]<bounds[1])
      si = 0;
    else
      si = 1;
  }
}


std::vector<Screen*> Screen::screens = std::vector<Screen*>();

Screen::Screen(){

  screens.push_back(this);

}

void Screen::genGraphics() { };
void Screen::update(float dt) { };


