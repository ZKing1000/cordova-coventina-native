#include "opengl.h"
#include <GLES2>
#include "shapes.h"

struct Texture : public SimpleShape {

  static GLuint texCoordBuf;

  static std::vector<Vec2> texCoords;

  static void init();


  GLuint texture;


  void genGraphics();

  void draw();

};
