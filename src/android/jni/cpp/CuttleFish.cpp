// vim: sw=4 expandtab

#include "../CuttleFish.h"

#include "screens/GameScreen.h"
#include "opengl/programs.h"
#include "CubeMesh.hpp"
#include "../read.h"

#include "../loop.h"

#include "../matrix.h"

#include "../extra.h"

std::vector<Shape*> CuttleFish::shapes;

TexturedShape* CuttleFish::poker;

unsigned short CuttleFish::pokerVertIndice = 0;

extern glm::ivec2 wellCenter;


CuttleFish::CuttleFish(glm::vec3 center)
    : center(center)
{
    pos = center;
    dir = {0.,0.,1.};
    speed = 1.;
    angle = 0.;

#if 0
    pokerPosInc = {0,0,0};

    inc(&pokerPosInc.z,1.);
#endif

};

void CuttleFish::genGraphics(){

  shapes = read::holden("CuttelFish.holden");

#if 0
  std::vector<Shape*> tmp = read::holden("Cube.holden");

  SDL_Log(("tmp size: "+toString(tmp.size())).c_str());

  poker = dynamic_cast<TexturedShape*>(tmp[0]);
#endif
}

void CuttleFish::update(float dt)
{
    pos += speed * dir * dt;
    if (glm::length(pos - center) > 1.6)
    {
        //printf("before: angle = %f, pos=%.2f,%.2f,%.2f,dir=%.2f,%.2f,%.2f\n", angle, pos[0],pos[1],pos[2],dir[0],dir[1],dir[2]);
        pos = center + dir * 1.6f;
        angle += M_PI + float(rand()) / RAND_MAX * 1;
        if (angle > M_PI * 2.)
            angle = angle - M_PI * 2.;
        //printf("after: angle = %f, pos=%.2f,%.2f,%.2f\n", angle, pos[0],pos[1],pos[2]);
    }

#if 0
  movePokerPos(
      pokerPosInc.x,
      pokerPosInc.y,
      pokerPosInc.z
  );

  pos.x+=pokerPosInc.z;
#endif
}

void CuttleFish::draw(){

  program::bind(program::LIGHTEN_TEXTURED_FIXED_COLOR);

  program::LightenTexturedFixedColor* p = static_cast<program::LightenTexturedFixedColor*>(program::getBound());

  auto prevModel = mat::model;
  mat::translate(pos.x,pos.y,pos.z);
  mat::rotate(angle, 0., 1., 0.);

  auto mvmat = mat::view*mat::model;
  dir = {mvmat[2][0],0.,mvmat[2][2]};

  glEnableVertexAttribArray(p->vertHandle);
  glEnableVertexAttribArray(p->texCoordHandle);
  glEnableVertexAttribArray(p->normalHandle);

  glUniform1i(p->doTexOffsetHandle,1);
  glUniform2f(p->texCoordOffsetHandle,GameScreen::instance->offsetX,GameScreen::instance->offsetY);
  glUniform1i(p->doLightUniform,0);
  glUniform1i(p->doLight2Uniform,0);

  //auto vec = mvmat * glm::vec4(0,0,0,1);
  //glUniform3f(p->lightPosUniform,vec.x,vec.y,vec.z);
  //glUniform1f(p->shininessUniform, 0.);
  //glUniform1i(p->doUniformNormalsUniform, 0);

  for(int i=0;i<shapes.size();i++)
    shapes[i]->draw();

  //poker->draw();

  mat::model = prevModel;
}

void CuttleFish::setPokerPos(float x,float y,float z){

  pokerPos.x = x;
  pokerPos.y = y;
  pokerPos.z = z;

  glBindBuffer(GL_ARRAY_BUFFER,poker->vertBuf);
  glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(Vec),&pokerPos);

  SDL_Log(("Poker pos z: "+toString(pokerPos.z)).c_str());

}

void CuttleFish::movePokerPos(float x,float y,float z){

  setPokerPos(
      pokerPos.x+x,
      pokerPos.y+y,
      pokerPos.z+z
  );

}
