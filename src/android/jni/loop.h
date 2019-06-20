#ifndef _LOOP_H
#define _LOOP_H

#include <vector>

#include "screen.h"


typedef float increment;


namespace loop{

  extern Screen* currentScreen;

  extern unsigned long tick;

  extern unsigned long prevTick;

  extern float calculatedFps;

  extern std::vector<float> incrementValues; //speed that maps to pointer so that
  extern std::vector<float*> incrementPointers; //it can change based on fps

  void init();
  void loop();

}

void inc(float* increment,float movementPerSecond);

#endif
