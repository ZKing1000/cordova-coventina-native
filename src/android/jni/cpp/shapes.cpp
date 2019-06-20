#include "../shapes.h"

#include <math.h>

#include "../matrix.h"

#include "../extra.h"

#include <SDL.h>

#include "../opengl/programs.h"

#include <sstream>

#include <glm/glm.hpp>

namespace shape{

  Shape* circle(float radius,std::vector<ColorRGB> colors,unsigned int outerVertNum,Shape* shape){

    if(shape == nullptr)
      shape = new Shape();

    if(outerVertNum == 0)
      outerVertNum = (unsigned int)((radius/0.1)*90);

    shape->verts = std::vector<Vec>(outerVertNum+1);

    Vec vec;
    vec.x = 0;
    vec.y = 0;
    vec.z = 0;

    shape->verts.push_back(vec);

    if(colors.size() > 0)
      shape->colors = colors;

    float radInc = (M_PI*2)/float(outerVertNum);

    float radCache = 0;
    for(int i=1;i<outerVertNum;i++){
      shape->verts[i].x = cos(radCache)*radius;
      shape->verts[i].y = sin(radCache)*radius;

      radCache+=radInc;
    }

    shape->indices = std::vector<unsigned short>((outerVertNum-1)*3);

    for(int i=3;i<(outerVertNum-1)*3;i+=3){

      shape->indices[i-3] = i/3;
      shape->indices[i-3+1] = (i/3)+1;
      shape->indices[i-3+2] = 0;

    }

    int tmp = (outerVertNum-2)*3;

    shape->indices[tmp] = 1;
    shape->indices[tmp+1] = shape->verts.size()-3;
    shape->indices[tmp+2] = 0;

    shape->type = CIRCLE;


  }


  namespace outline{

    SimpleShape* circle(float radius,std::vector<ColorRGB> colors,unsigned int vertNum,SimpleShape* shape){

      if(shape == nullptr)
	shape = new SimpleShape();

      if(vertNum == 0)
	vertNum = (unsigned int)((radius/0.1)*90);

      shape->verts = std::vector<Vec>(vertNum);

      if(colors.size() > 0)
	shape->colors = colors;

      float radInc = (M_PI*2)/float(vertNum);

      float radCache = 0;
      for(int i=0;i<vertNum;i++){
	shape->verts[i].x = cos(radCache)*radius;
	shape->verts[i].y = sin(radCache)*radius;

	radCache+=radInc;
      }

      shape->type = OUTLINE_CIRCLE;


      return shape;
    }

  }

}


std::vector<SimpleShape*> SimpleShape::instances = std::vector<SimpleShape*>();

SimpleShape::SimpleShape(ColorRGB color,bool multipleColors){

  instances.push_back(this);

  if(!multipleColors){
    colors = std::vector<ColorRGB>(1);
    colors[0] = color;
  }

}

void SimpleShape::draw(){

  if(verts.size() == colors.size()){ //draw colors



  }else{ //assumes colors.size() == 1 and a the bound program is fixedColor

    //glUniform4fv(program::fixedColor::colorHandle,1,&colors[0].r);

  }

  mat::translate(pos.x,pos.y,pos.z);

  mat::downloadMVP();

  mat::translate(-pos.x,-pos.y,-pos.z);

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glVertexAttribPointer(program::getBound()->vertHandle,3,GL_FLOAT,false,0,0);

  glUniform4fv(program::getBound()->colorHandle,1,&colors[0].r);

  glDrawArrays(drawMode,0,verts.size());


}

void SimpleShape::genGraphics(){

  glGenBuffers(1,&vertBuf);
  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);
  glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(Vec),&verts[0].x,GL_DYNAMIC_DRAW);

}

void TexturedSimpleShape::genGraphics(){

  SimpleShape::genGraphics();

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

}

void TexturedSimpleShape::draw(){

  mat::translate(pos.x,pos.y,pos.z);

  mat::downloadMVP();

  mat::translate(-pos.x,-pos.y,-pos.z);

  program::TexturedFixedColor* p = static_cast<program::TexturedFixedColor*>(program::getBound());

  glUniform4fv(program::getBound()->colorHandle,1,&colors[0].r);

  glBindBuffer(GL_ARRAY_BUFFER,texCoordBuf);

  glVertexAttribPointer(p->texCoordHandle,2,GL_FLOAT,GL_FALSE,0,0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glUniform1i(p->textureUniform, 0);


  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glVertexAttribPointer(p->vertHandle,3,GL_FLOAT,false,0,0);

  glDrawArrays(drawMode,0,verts.size());

}

Shape::Shape(ColorRGB color,bool multipleColors){

  drawMode = GL_TRIANGLES;

}

void Shape::genGraphics(){

  SimpleShape::genGraphics();

  glGenBuffers(1,&idxBuf);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,idxBuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned short),&indices[0],GL_DYNAMIC_DRAW);

}

void Shape::draw(){

  if(verts.size() == colors.size()){ //draw colors



  }else{ //assumes colors.size() == 1 and a the bound program is fixedColor

    //glUniform4fv(program::fixedColor::colorHandle,1,&colors[0].r);

  }

  mat::translate(pos.x,pos.y,pos.z);

  mat::downloadMVP();

  mat::translate(-pos.x,-pos.y,-pos.z);

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glVertexAttribPointer(program::getBound()->vertHandle,3,GL_FLOAT,false,0,0);

  glUniform4fv(program::getBound()->colorHandle,1,&colors[0].r);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,idxBuf);

  glDrawElements(drawMode,indices.size(),GL_UNSIGNED_SHORT,0);

  /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

  ColorRGB color;
  color.r = 0.5;
  color.g = 1;
  color.b = 0.5;

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glVertexAttribPointer(program::fixedColor::vertHandle,2,GL_FLOAT,false,0,0);

  glUniform4fv(program::fixedColor::colorHandle,1,&color.r);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,tmp);  

  glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_SHORT,0);*/


  //glDrawArrays(drawMode,0,verts.size());

  //SDL_Log(("verts.size: "+std::to_string(verts.size())+" indices.size: "+std::to_string(indices.size())+" colors.size: "+std::to_string(colors.size())+" mode equals: "+std::to_string(GL_TRIANGLES == drawMode)+" pos: ("+std::to_string(pos.x)+","+std::to_string(pos.y)+") vertBuf: "+std::to_string(vertBuf)+" idx buf: "+std::to_string(idxBuf)+" color: ("+std::to_string(colors[0].r)+","+std::to_string(colors[0].g)+","+std::to_string(colors[0].b)).c_str());

}

void TexturedShape::genGraphics(){

  Shape::genGraphics();

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


}

void TexturedShape::draw(){

  mat::translate(pos.x,pos.y,pos.z);

  mat::downloadMVP();

  mat::translate(-pos.x,-pos.y,-pos.z);

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glVertexAttribPointer(program::getBound()->vertHandle,3,GL_FLOAT,false,0,0);

  glUniform4fv(program::getBound()->colorHandle,1,&colors[0].r);

  glBindBuffer(GL_ARRAY_BUFFER,texCoordBuf);

  program::TexturedFixedColor* p = static_cast<program::TexturedFixedColor*>(program::getBound());
  glVertexAttribPointer(p->texCoordHandle,2,GL_FLOAT,GL_FALSE,0,0);

  glActiveTexture(GL_TEXTURE0);
  //SDL_Log("10 GL errors: %s",gluErrorString(glGetError()));
  glBindTexture(GL_TEXTURE_2D, texture);
  //SDL_Log("11 GL errors: %s",gluErrorString(glGetError()));
  glUniform1i(p->textureUniform, 0);


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,idxBuf);

  glDrawElements(drawMode,indices.size(),GL_UNSIGNED_SHORT,0);

}

void LightenTexturedShape::draw(){

  program::LightenTexturedFixedColor* p = static_cast<program::LightenTexturedFixedColor*>(program::getBound());


  mat::translate(pos.x,pos.y,pos.z);

  mat::downloadMVP();

  glUniformMatrix4fv(p->mvmUniform, 1, GL_FALSE, glm::value_ptr(mat::view*mat::model));

  glUniform3f(p->lightPosUniform,lightPos.x,lightPos.y,lightPos.z);

  mat::translate(-pos.x,-pos.y,-pos.z);

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glVertexAttribPointer(program::getBound()->vertHandle,3,GL_FLOAT,false,0,0);

  glUniform4fv(program::getBound()->colorHandle,1,&colors[0].r);

  glBindBuffer(GL_ARRAY_BUFFER,texCoordBuf);

  glVertexAttribPointer(p->texCoordHandle,2,GL_FLOAT,GL_FALSE,0,0);

  glBindBuffer(GL_ARRAY_BUFFER,normalBuf);
  glVertexAttribPointer(p->normalHandle,3,GL_FLOAT,GL_FALSE,0,0);

  glActiveTexture(GL_TEXTURE0);
  //SDL_Log("10 GL errors: %s",gluErrorString(glGetError()));
  glBindTexture(GL_TEXTURE_2D, texture);
  //SDL_Log("11 GL errors: %s",gluErrorString(glGetError()));
  glUniform1i(p->textureUniform, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,idxBuf);

  glDrawElements(drawMode,indices.size(),GL_UNSIGNED_SHORT,0);

}

void LightenTexturedShape::genGraphics(){

  TexturedShape::genGraphics();

  glGenBuffers(1,&normalBuf);
  glBindBuffer(GL_ARRAY_BUFFER,normalBuf);
  glBufferData(GL_ARRAY_BUFFER,normals.size()*sizeof(Vec),&normals[0],GL_DYNAMIC_DRAW);

}


