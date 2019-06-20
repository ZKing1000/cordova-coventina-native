#include "../SoftDude.h"

#include <math.h>

#include <cstdlib>
#include <climits>

#include <string>
#include <SDL.h>

#include "extra.h"

#define _2PI 2*M_PI

int SoftDude::glProgram = 0;

SoftDude::SoftDude(){
  int number = 75;

  verts = std::vector<vec2>(number);

  angularVerts = std::vector<AngularVelocity>(number-1);

  verts[0].y = -0.5;

  float cache = M_PI*0.25;

  float dec = ((M_PI*0.75)-(M_PI*0.25))/float(number-1);

  for(int i=0;i<angularVerts.size();i++){
    AngularVelocity* a = &angularVerts[i];

    a->angle = cache;

    if(a->angle == 0)
      a->angle = M_PI*0.99;

    cache-=dec;

    if(cache<0)
      cache+=_2PI;

    if(a->angle<M_PI){
      a->accel = -a->accel;
      a->resistance = -a->resistance;
    }

    verts[i+1].x = sin(angularVerts[i].angle)*maxTension;
    verts[i+1].y = cos(angularVerts[i].angle)*maxTension;
  }

}

void SoftDude::genBuf(){
  glGenBuffers(1,&vertBuf);
  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);
  glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(float),&verts[0],GL_DYNAMIC_DRAW);
}

void SoftDude::draw(){

  /*if((rand()%120)==0){
    verts = std::vector<vec2>(rand()%90);

    float angle = 0;
    float angleInc = (M_PI*2)/verts.size();
    for(int i=0;i<verts.size();i++){
      verts[i].x = cos(angle)*size;
      verts[i].y = sin(angle)*size;

      angle+=angleInc;
    }
  }*/

  for(int i=1;i<verts.size();i++){
    vec2* v = &verts[i];
    AngularVelocity* a = &angularVerts[i-1];

    /*a->speed+=a->accel;
    a->speed+=a->resistance;
    a->angle+=a->speed;

    if(fabs(fabs(a->angle-M_PI)-M_PI)<fabs(a->speed)){
      a->speed+=a->accel;
      a->accel = -a->accel;
      a->resistance = -a->resistance;
      a->angle = 0;


      if(fabs(fabs(a->speed)-((fabs(a->accel)+fabs(a->resistance))*2)) <= 0.005){
	a->speed = 0;
	a->angle = 0;
	a->accel = 0;
	a->resistance = 0;
      }
    }

    if(a->angle<0)
      a->angle+=_2PI;
    else if(a->angle>_2PI)
      a->angle-=_2PI;*/

    v->x = verts[i-1].x+(sin(a->angle)*maxTension);
    v->y = verts[i-1].y+(cos(a->angle)*maxTension);
  }

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);
  glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(vec2),&verts[0].x,GL_DYNAMIC_DRAW);

  glVertexAttribPointer(program::fixedColor::vertHandle,dimension,GL_FLOAT,false,0,0);

  glUniform4fv(program::fixedColor::colorHandle,1,color);

  glDrawArrays(GL_LINE_STRIP,0,verts.size());

  glUniform1f(glGetUniformLocation(program::fixedColor::id,"m_gl_PointSize"),10);

  glUniform4fv(program::fixedColor::colorHandle,1,color2);

  //glDrawArrays(GL_POINTS,0,verts.size());


}
