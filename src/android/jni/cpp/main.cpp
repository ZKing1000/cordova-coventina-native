#include "../main.h"

#include "../screen.h"

#include "../loop.h"

#include "../opengl/programs.h"

#include "../audio.h"

int main( int argc, char* args[] ){
  SDL_Init(SDL_INIT_VIDEO);

  srand(time(0));

  screen::init();

  audio::init();

  gl::init();

  program::init();

  loop::init();

  while(true)
    loop::loop();


}
