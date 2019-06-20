#include "../loop.h"

#include <SDL.h>

#include "../extra.h"

#include "../screen.h"

#include "../screens/GameScreen.h"

float avgFps = 0.;

namespace loop{

  Screen* currentScreen = GameScreen::instance;

  unsigned long tick = 0;
  unsigned long tickCounter = 0;
  float fpsSum = 0.0;

  unsigned long prevTick;

  float calculatedFps;

  std::vector<float> incrementValues;
  std::vector<float*> incrementPointers;

  void init() {
    tick = SDL_GetTicks();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(screen::window);
  }

  void loop(){

    prevTick = tick;
    tick = SDL_GetTicks();

    int dif = tick-prevTick;
    if (dif == 0) return;

    float dt = dif * 0.001;
    calculatedFps = 1.0 / dt;

    constexpr bool showFps = true;
    if (showFps)
    {
      fpsSum += calculatedFps;
      if (++tickCounter == 120)
      {
        avgFps = fpsSum / 120;
        fpsSum = 0.;
        tickCounter = 0;

        //SDL_Log("calculated fps: %.1f;  pixelBounds: %d, %d", avgFps, screen::pixelBounds[0], screen::pixelBounds[1]);
      }
    }

    for(int i=0;i<incrementValues.size();i++)
      *incrementPointers[i] = incrementValues[i]/calculatedFps;

    currentScreen->update(dt);

    SDL_GL_SwapWindow(screen::window);

  }

}

void inc(float* increment,float movementPerSecond){

  loop::incrementValues.push_back(movementPerSecond);

  loop::incrementPointers.push_back(increment);

}
