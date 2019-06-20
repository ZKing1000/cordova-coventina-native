#include "shapes.h"
#ifndef _THUMBSTICK_H
#define _THUMBSTICK_H

#include "opengl/opengl.h"
#include "ui/InputEvent.h"

struct Thumbstick {

  Vec2 pos;

  Texture* innerCircle;

  SimpleShape* outerRing;
  bool active = false;

  Vec2 direction;
  float radius = 0.2;
  float innerCirclePosLength;


  Thumbstick();

  void draw();

  void update();

  void genGraphics();

  void keyInput(SDL_Event e,InputEvent* ie);

  void mouseHandling(InputEvent* e);

};

#endif
