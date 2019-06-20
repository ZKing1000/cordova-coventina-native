#include "Surface.h"
#include <SDL.h>
#include <SDL_image.h>
#include "../fileSystem.h"

Surface::Surface(std::string path){ 

  path = fileSystem::getAssetsPath() + path;

  SDL_Surface* s = IMG_Load(path.c_str()); //memory leak that I have to deal with

  if(s == nullptr){
    SDL_Log(("CANNOT LOAD IMAGE: "+path).c_str());
    exit(0);
  }

  w = s->w;
  h = s->h;
  components = s->format->BytesPerPixel;

  data = new unsigned char[s->w*s->h*components];

  unsigned char* pixels = (unsigned char*) s->pixels;

  memcpy(data,pixels,s->w*s->h*components);
}
