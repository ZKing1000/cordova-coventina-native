#ifndef _GAMESCREEN_H
#define _GAMESCREEN_H

#include "../screen.h"

#include "../SoftBody.h"

#include "../shapes.h"

#include "../opengl/opengl.h"

#include "audio.h"

#include "CuttleFish.h"

#include <SDL_image.h>

#include "glm/glm.hpp"

#include "../MachineGun.h"

#include "ui/InputEvent.h"
#include "ui/Thumbstick.h"


struct GameScreen : public Screen {

  static GameScreen* instance;


  bool mouseButtonDown = false;

  static Sound* pickupSound;
  static Sound* dropSound;
  static Sound* jumpSound;
  static Sound* winnerSound;
  static Sound* loserSound;

  static InputEvent* inputEvent;

  static Thumbstick* thumbstick;

  CuttleFish* cuttleFish;

  float timeleft;

  float timeMult = 90;

  static unsigned level;


  //HeldMachineGun gun;

//

  float offsetX = 0;
  float offsetY = 0;

//

  GameScreen();

  void genGraphics();

  void keyInput();

  void render(float dt);

  void update(float dt);

};



#endif
