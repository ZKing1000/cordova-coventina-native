// vim: sw=2 expandtab
#include "../matrix.h"

#include "../opengl/opengl.h"
#include "../opengl/programs.h"

namespace mat{

  glm::mat4 projection = glm::mat4(1);
  glm::mat4 view = glm::mat4(1);
  glm::mat4 model = glm::mat4(1);

  void downloadMVP(){
    auto mvp = projection*view*model;
    glUniformMatrix4fv(glGetUniformLocation(program::getBound()->id, "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
  }

  void clearMatrices(){
    projection = glm::mat4(1);
    view = glm::mat4(1);
    model = glm::mat4(1);
  }

  void ortho(float left,float right,float bottom,float top,float near,float far){
    projection = glm::ortho(left,right,bottom,top,near,far);
  }

  void perspective(float fov,float aspectRatio,float nearClip,float farClip){
    projection = glm::perspective(fov,aspectRatio,nearClip,farClip);
  }

  void translate(float x,float y,float z){
    model = glm::translate(model,glm::vec3(x,y,z));
  }

  void rotate(float radians,int x,int y,int z){
    model = glm::rotate(model,radians,glm::vec3(x,y,z));
  }

  void scale(float x,float y,float z){
    model = glm::scale(model,glm::vec3(x,y,z));
  }

}
