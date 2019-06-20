#ifndef CUTTLEFISH_HPP_
#define CUTTLEFISH_HPP_

#include "shapes.h"
#include "glm/vec3.hpp"

#include <vector>

struct CuttleFish {

  static std::vector<Shape*> shapes;

  static TexturedShape* poker;

  static unsigned short pokerVertIndice;

  glm::vec3 center;
  glm::vec3 pos;
  glm::vec3 dir;
  float speed;
  float angle;

  Vec pokerPos;

  Vec pokerPosInc;


  CuttleFish(glm::vec3 center);

  void update(float dt);

  void draw();

  void setPokerPos(float x,float y,float z);

  void movePokerPos(float x,float y,float z);

  static void genGraphics();

};

#endif

