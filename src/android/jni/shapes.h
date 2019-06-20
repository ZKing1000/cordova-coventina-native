
#ifndef _SHAPES_H
#define _SHAPES_H

#include "opengl/opengl.h"
#include "Surface.h"
#include <vector>

struct SimpleShape;

struct Shape;

namespace shape{

  enum EnumShape {

    CIRCLE,
    OUTLINE_CIRCLE

  };

  Shape* circle(float radius=0.2,std::vector<ColorRGB> colors=std::vector<ColorRGB>(0),unsigned int outerVertNum=0,Shape* shape=nullptr);

  namespace outline{

    SimpleShape* circle(float radius=0.2,std::vector<ColorRGB> colors=std::vector<ColorRGB>(0),unsigned int vertNum=0,SimpleShape* shape=nullptr);

  }

}

struct SimpleShape{

  static std::vector<SimpleShape*> instances;

  GLuint vertBuf;

  std::vector<Vec> verts;

  std::vector<ColorRGB> colors; //when using the fixed color shader you can use this to store one value

  GLenum drawMode = GL_LINE_LOOP;

  shape::EnumShape type;

  Vec pos = {0,0,0};


  SimpleShape(ColorRGB color=ColorRGB(1,0,0),bool multipleColors=false);

  virtual void draw();

  virtual void genGraphics();

};

struct TexturedSimpleShape : public SimpleShape {

  GLuint texCoordBuf;

  GLuint texture;

  std::vector<Vec2> texCoords;

  Surface* surface;

  void genGraphics();

  void draw();

};

typedef TexturedSimpleShape Texture;

struct Shape : public SimpleShape{

  Shape(ColorRGB color=ColorRGB(1,0,0),bool multipleColors=false);

  GLuint idxBuf;

  std::vector<unsigned short> indices;

  void genGraphics();

  void draw();

};

struct TexturedShape : public Shape {

  GLuint texCoordBuf;

  GLuint texture;

  std::vector<Vec2> texCoords;

  Surface* surface;

  void genGraphics();

  void draw();

};

struct LightenTexturedShape : TexturedShape {

  GLuint normalBuf;

  std::vector<Vec> normals;

  Vec lightPos = {0,0,0};

  void genGraphics();

  void draw();

};

#endif
