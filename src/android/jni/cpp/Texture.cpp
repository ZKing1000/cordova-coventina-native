/*#include "Texture.h"

GLuint Texture::texCoordBuf;

std::vector<Vec2> Texture::texCoords;

void Texture::init(){

  glGenBuffers(1,&texCoordBuf);
  glBindBuffer(GL_ARRAY_BUFFER,texCoordBuf);
  glBufferData(GL_ARRAY_BUFFER,texCoords.size()*sizeof(Vec2),&texCoords[0],GL_DYNAMIC_DRAW);


  glEnable(GL_TEXTURE_2D);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  if (surface->components == 3)
  {
	  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->data);
  }
  else
  {
	  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->data);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glDisable(GL_TEXTURE_2D);

}*/
