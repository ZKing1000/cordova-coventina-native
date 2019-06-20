#ifndef _OPENGL_H
#define _OPENGL_H

#include "glcommon.h"

#include <vector>

#include <string>

namespace gl{

  void init();

}

struct ColorRGB{
  float r=1;
  float g=1;
  float b=1;

  ColorRGB(float r=1,float g=1,float b=1);

};

struct Vec{
  float x;
  float y;
  float z;
};

struct Vec2{
  float x;
  float y;
};

struct AngularVelocity{
  static float cache;

  float angle;
  float speed;
  float accel=0.001; //acceleration
  float resistance=0.01;

  AngularVelocity();
};

struct DrawCache{
  GLuint vertBuf = 0;
  GLuint colorBuf = 0;

  int dimension = 2; //2D or 3D
  int bytesPerColor = 4; //3 or 4
  GLenum mode = GL_LINE_LOOP;

  std::vector<float> verts;
  std::vector<float> colors;

  virtual void genBuf();

  void reallocateBuf();

  void downloadBuf();

  virtual void draw();

  void update();
};

#endif
