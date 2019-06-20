#ifndef _SCREEN_H
#define _SCREEN_H

#include <SDL.h>

#include <vector>

namespace screen{

  extern SDL_Window* window;
  extern SDL_GLContext gl;

  extern float bounds[2];

  extern int si; // index to bounds that represents which is smaller

  extern int pixelBounds[2];

  void init();

}

struct Screen {  //like a view in the java sdk. lol f java

  static std::vector<Screen*> screens;

  Screen();

  virtual void genGraphics();

  virtual void update(float dt);

};

#endif
