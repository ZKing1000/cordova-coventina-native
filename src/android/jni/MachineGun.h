#include "opengl/opengl.h"

#include "shapes.h"

#include <SDL.h>

struct HeldMachineGun {

  Vec pos;

  std::vector<Shape*> scene;

  bool firing = false;

  float timeTillNextFireCache;

  static float timeTillNextFireInc;


  HeldMachineGun();

  void keyInput(SDL_Event e);

  void update();

  void draw();

  void fire();

};

struct Bullet {

  static std::vector<Bullet*> instances;

  static LightenTexturedShape* shape;

  static void genGraphics();

  Vec pos;

  Vec velocity;

  Vec acceleration;

  Vec accelerationScalar;

  void update();

  void draw();

};
