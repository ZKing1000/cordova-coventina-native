#include "../SoftBody.h"

#include "../opengl/programs.h"

#include <cstdlib>

#include <limits.h>

const float PI_2 = M_PI*2;

SoftBody::SoftBody(){
  center.x = 0.75;
  center.y = 0.2;

  int vertNum = 60;

  verts = std::vector<Vec>(vertNum);
  angles = std::vector<float>(vertNum);
  radiuss = std::vector<float>(vertNum);

  for(int i=0;i<radiuss.size();i++){
    radiuss[i] = favorableRadius*(rand()/float(INT_MAX));
  }
}

void SoftBody::update(){

  float angleInc = PI_2/verts.size();

  float angle = 0;

  for(int i=0;i<verts.size();i++){
    Vec* v = &verts[i];
    float* a = &angles[i];
    float* r = &radiuss[i];

    float inc = ((favorableRadius-(*r))/favorableRadius)*compression;

    if(fabs(*r-favorableRadius)<inc){
      *r = favorableRadius;
      inc = 0;
    }

    *r+=inc;

    *a = angle;

    angle+=angleInc;

    v->x = cos(*a)*(*r);
    v->y = sin(*a)*(*r);


  }



}

void SoftBody::genBuf(){
  glGenBuffers(1,&vertBuf);
  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);
  glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(float),&verts[0],GL_DYNAMIC_DRAW);
}

void SoftBody::draw(){

  update();

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);
  glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(Vec),&verts[0].x,GL_DYNAMIC_DRAW);

  glVertexAttribPointer(program::fixedColor::vertHandle,dimension,GL_FLOAT,false,0,0);

  glUniform4fv(program::fixedColor::colorHandle,1,color);

  glDrawArrays(GL_LINE_LOOP,0,verts.size());

  glUniform1f(glGetUniformLocation(program::fixedColor::id,"m_gl_PointSize"),10);

  glUniform4fv(program::fixedColor::colorHandle,1,color2);

  glDrawArrays(GL_POINTS,0,verts.size());

  std::vector<Vec> points = std::vector<Vec>(1);

  points[0] = center;

  glBindBuffer(GL_ARRAY_BUFFER,NULL);

  glVertexAttribPointer(program::fixedColor::vertHandle,dimension,GL_FLOAT,false,sizeof(Vec),&points[0].x);

  glDrawArrays(GL_POINTS,0,points.size());

}
