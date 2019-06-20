// vim: sw=2 expandtab
#include "../../opengl/opengl.h"

#include <math.h>
#include <cstdlib>

#include <string>

#include "../../screen.h"

#include "../../shapes.h"
#include "../../fileSystem.h"
#include "../../Surface.h"

#include <SDL_image.h>

#include <cstring>

namespace gl{

  void init(){

    glViewport(0,0,screen::pixelBounds[0],screen::pixelBounds[1]);

    //glClearColor(0.5,1,0.5,1);
    //
    glClearColor(0.0,0.0,0.0,1);

    glEnable(GL_DEPTH_TEST);

    for(int i=0;i<Screen::screens.size();i++)
      Screen::screens[i]->genGraphics();

    Surface* defaultSurface = new Surface("MapTexture.png");

    for(int i=0;i<SimpleShape::instances.size();i++){

      if(TexturedShape* shape = dynamic_cast<TexturedShape*>(SimpleShape::instances[i])){
        if (shape->surface == nullptr)
          shape->surface = defaultSurface;
      }

      SimpleShape::instances[i]->genGraphics();
    }

  }

}

ColorRGB::ColorRGB(float r,float g,float b){
  this->r = r;
  this->g = g;
  this->b = b;
}

float AngularVelocity::cache = 0;

AngularVelocity::AngularVelocity(){
  /*angle = (rand()/float(INT_MAX))*(2*M_PI);

  AngularVelocity::cache+=0.2;

  if(cache>=(M_PI*2))
    cache = 0;

  if(angle == M_PI)
    angle = 1.5;
  else if(angle == 0)
    angle = 1.5;

  if(angle<M_PI){
    accel = -accel;
    resistance = -resistance;
  }*/
}
