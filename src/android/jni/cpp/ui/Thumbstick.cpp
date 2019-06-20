// vim: sw=4 expandtab
#include "ui/Thumbstick.h"
#include "Surface.h"
#include "matrix.h"
#include "screen.h"

#include "opengl/programs.h"
#include "extra.h"
#include <math.h>
#include "Player.hpp"

void Thumbstick::draw()
{
    mat::ortho(-screen::bounds[0],screen::bounds[0],screen::bounds[1],-screen::bounds[1],-1,1);

    mat::translate(pos.x,pos.y,0);

    mat::downloadMVP();

    program::bind(program::TEXTURED_FIXED_COLOR);

    program::TexturedFixedColor* p2 = static_cast<program::TexturedFixedColor*>(program::getBound());

    glEnableVertexAttribArray(p2->vertHandle);
    glEnableVertexAttribArray(p2->texCoordHandle);

    innerCircle->draw();

    program::bind(program::FIXED_COLOR);
    program::FixedColor* p = static_cast<program::FixedColor*>(program::getBound());
    glEnableVertexAttribArray(p->vertHandle);
    mat::downloadMVP();

    outerRing->draw();

    mat::translate(-pos.x,-pos.y,0);
}

Thumbstick::Thumbstick(){

  
}

void Thumbstick::genGraphics(){

  innerCircle = new Texture();

  innerCircle->surface = new Surface("ui/thumbstick.png");
  innerCircle->drawMode = GL_TRIANGLES;

  float size = 0.1;

  Vec verts[4] = {
    -0.1,-0.1,0,
    0.1,-0.1,0,
    0.1,0.1,0,
    -0.1,0.1,0
  };

  Vec2 texCoords[4] = {
    0,0,
    1,0,
    1,1,
    0,1
  };

  innerCircle->verts.push_back(verts[0]);
  innerCircle->verts.push_back(verts[1]);
  innerCircle->verts.push_back(verts[2]);
  innerCircle->verts.push_back(verts[3]);

  innerCircle->texCoords.push_back(texCoords[0]);
  innerCircle->texCoords.push_back(texCoords[1]);
  innerCircle->texCoords.push_back(texCoords[2]);
  innerCircle->texCoords.push_back(texCoords[3]);

  outerRing = shape::outline::circle();

}

void Thumbstick::update(){

  if(InputEvent::instance->pressed)
    SDL_Log(("PRESSED"));
  if(active)
    SDL_Log(("ACTIVE"));

  if(InputEvent::instance->pressed && active){

    Vec2 vec;
    vec.x = direction.x * (innerCirclePosLength/radius);
    vec.y = direction.y * (innerCirclePosLength/radius);

    game::player.runDirection(vec);

  }

}

void Thumbstick::keyInput(SDL_Event e,InputEvent* ie){

  switch(e.type){
    case SDL_MOUSEBUTTONDOWN:
      {
      
      float difX = ie->x-outerRing->pos.x;
      float difY = ie->y-outerRing->pos.y;

      float pathag = sqrt(pow(fabs(difX),2)+pow(fabs(difY),2));

      if(pathag<=radius){
	game::player.zRunning = true;
	game::player.xRunning = true;
	active = true;
      }


      mouseHandling(ie);
      }
      break;
    case SDL_MOUSEBUTTONUP:
      active = false;
      game::player.stopRunning();
      break;
    case SDL_MOUSEMOTION:
      mouseHandling(ie);
      break;
  }

}

void Thumbstick::mouseHandling(InputEvent* e){

  float difX = e->x-outerRing->pos.x;
  float difY = e->y-outerRing->pos.y;

  float pathag = sqrt(pow(fabs(e->x-outerRing->pos.x),2)+pow(fabs(e->y-outerRing->pos.y),2));

  innerCirclePosLength = pathag;

  float x = difX/pathag;
  float y = difY/pathag;

  direction.x = x;
  direction.y = -y;

  SDL_Log(("X: "+toString(difX)+" Y: "+toString(difY)+" pathag: "+toString(pathag)+" hypot: "+toString(hypot(fabs(difX-pos.x),fabs(difY-pos.y)))).c_str());

  if(hypot(fabs(difX-pos.x),fabs(difY-pos.y))<=radius){
    innerCircle->pos.x = e->x;
    innerCircle->pos.y = e->y;
  }else{
    //SDL_Log(("atan: "+toString(atan(fabs(y-outerMovementRing->pos.y)/fabs(x-outerMovementRing->pos.x)))).c_str());
    //normalizing the vector

    innerCircle->pos.x = (x*radius)+outerRing->pos.x;
    innerCircle->pos.y = (y*radius)+outerRing->pos.y;
  }

}

