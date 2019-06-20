#include "ui/InputEvent.h"

#include "screen.h"

InputEvent* InputEvent::instance;


InputEvent::InputEvent(){

  instance = this;

}

void InputEvent::update(){

  SDL_GetMouseState(&px,&py);

  x = ((px/float(screen::pixelBounds[0]))*2)-1;
  y = ((py/float(screen::pixelBounds[1]))*(screen::bounds[1]*2))-screen::bounds[1];

}
