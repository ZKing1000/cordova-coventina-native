
#include "opengl.h"
#include <vector>

struct SoftDude{

  float speed = 0.005;

  float resistance = 0.001;

  float grav = 0.001;

  float maxTension = 0.01;

  static int glProgram; //this is set before and draw()s are called. I plan on improving the infastructure so it wont work like that

  int dimension = 2;
  std::vector<vec2> verts;

  std::vector<AngularVelocity> angularVerts;

  float color[3] = {0.75,0.75,0.75};
  float color2[3] = {0,1,0};

  GLuint vertBuf;



  float size = 0.5;

  float inc = 0.005;

  float x = 0;

  SoftDude();

  void genBuf();

  void draw();
};
