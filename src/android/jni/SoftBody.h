#include <vector>

#include <math.h>

#include "opengl/opengl.h"

struct SoftBody{

  GLuint vertBuf;

  int dimension = 2;

  Vec center;

  float tension = 0.1;

  float favorableRadius = 0.4;

  float compression = 0.01;

  float color[3] = {0.75,0.75,0.75};
  float color2[3] = {0,1,0};

  std::vector<Vec> verts;

  std::vector<float> angles;

  std::vector<float> radiuss;

  SoftBody();

  void update();

  void genBuf();

  void draw();

};
