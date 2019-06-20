#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace mat{

  extern glm::mat4 projection;
  extern glm::mat4 view;
  extern glm::mat4 model;

  void downloadMVP(); //updates MVP matrix in bound shader with projection*view*model

  void clearMatrices();

  void ortho(float left=-1,float right=1,float bottom=1,float top=-1,float near=-1,float far=1);

  void perspective(float fov,float aspectRatio,float nearClip=0.1,float farClip=100);

  void translate(float x,float y,float z);

  void rotate(float radians,int x,int y,int z);

  void scale(float x,float y,float z);

}
