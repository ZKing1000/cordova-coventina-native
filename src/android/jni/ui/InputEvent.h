#ifndef _INPUTEVENT_H
#define _INPUTEVENT_H

#include <SDL.h>

struct InputEvent {

  static InputEvent* instance;

  float x;
  float y;

  int px;
  int py;

  bool pressed;

  InputEvent();

  void update();

};

#endif
