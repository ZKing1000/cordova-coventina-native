#include "../../screens/GameScreen.h"

#include <SDL.h>

#include "../../opengl/programs.h"

#include "../../matrix.h"

#include "../../loop.h"

#include "../../extra.h"

Screen* GameScreen::instance = new GameScreen();

GameScreen::GameScreen(){

  loop::currentScreen = this;

}  

void GameScreen::genGraphics(){

   texData = new SimpleSurface("background3.jpg");

  float verts[2*6] = {
    -screen::bounds[0]/div,-screen::bounds[1]/div,
    -screen::bounds[0]/div,screen::bounds[1]/div,
    screen::bounds[0]/div,screen::bounds[1]/div,

    -screen::bounds[0]/div,-screen::bounds[1]/div,
    screen::bounds[0]/div,-screen::bounds[1]/div,
    screen::bounds[0]/div,screen::bounds[1]/div
  };

  glGenBuffers(1,&vertBuf);
  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);
  glBufferData(GL_ARRAY_BUFFER,6*2*sizeof(float),&verts[0],GL_STATIC_DRAW);

  glGenBuffers(1,&texCoordBuf);
  glBindBuffer(GL_ARRAY_BUFFER,texCoordBuf);
  glBufferData(GL_ARRAY_BUFFER,6*2*sizeof(float),&texCoords[0],GL_STATIC_DRAW);

  //texData = SimpleSurface("../../images/image.png");

  glEnable(GL_TEXTURE_2D);

  glGenTextures(1, &texture0);
  glBindTexture(GL_TEXTURE_2D, texture0);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, texData->w, texData->h, 0, GL_RGB, GL_UNSIGNED_BYTE, texData->data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glDisable(GL_TEXTURE_2D);

  body.genBuf();

  float offset = screen::bounds[1]/2;

  float radius = 0.2;

  outerMovementRing = shape::outline::circle(radius);

  outerMovementRing->pos.x = screen::bounds[0]-offset;
  outerMovementRing->pos.y = screen::bounds[1]-offset;

  innerMovementCircle = shape::circle(radius/2);

  innerMovementCircle->pos.x = outerMovementRing->pos.x;
  innerMovementCircle->pos.y = outerMovementRing->pos.y;

}

void GameScreen::keyInput(){

  SDL_Event event;

  while(SDL_PollEvent(&event)){

    float bounds[2] = {screen::bounds[0],screen::bounds[1]};

    float x;
    float y;

    /*x = (event.tfinger.x*2)-1;
    y = (event.tfinger.y*(bounds[1]*2))-bounds[1];*/ //removed for computer compatibility

    int px, py;
    SDL_GetMouseState(&px, &py);

    x = ((px/float(screen::pixelBounds[0]))*2)-1;
    y = ((py/float(screen::pixelBounds[1]))*(bounds[1]*2))-bounds[1];

    switch(event.type){
      case SDL_QUIT:
	exit(0);
	break;
      case SDL_MOUSEBUTTONDOWN:
	{
	down = true;


	float difX = x-outerMovementRing->pos.x;
	float difY = y-outerMovementRing->pos.y;

	float pathag = sqrt(pow(fabs(x-outerMovementRing->pos.x),2)+pow(fabs(y-outerMovementRing->pos.y),2));

	float x = difX/pathag;
	float y = difY/pathag;








	if(hypot(fabs(x-outerMovementRing->pos.x),fabs(y-outerMovementRing->pos.y))<=0.2){
	  innerMovementCircle->pos.x = x;
	  innerMovementCircle->pos.y = y;
	}else{
	  //SDL_Log(("atan: "+toString(atan(fabs(y-outerMovementRing->pos.y)/fabs(x-outerMovementRing->pos.x)))).c_str());
	  //normalizing the vector
	  
	  SDL_Log(("THIS SHOULD BE 1: "+toString(fabs(x)+fabs(y))).c_str());

	  innerMovementCircle->pos.x = (x*0.2)+outerMovementRing->pos.x;
	  innerMovementCircle->pos.y = (y*0.2)+outerMovementRing->pos.y;
	}
	break;
      }

      case SDL_MOUSEBUTTONUP:
	down = false;
	break;
      case SDL_MOUSEMOTION:
	if(hypot(fabs(x-outerMovementRing->pos.x),fabs(y-outerMovementRing->pos.y))<=0.2){
	  innerMovementCircle->pos.x = x;
	  innerMovementCircle->pos.y = y;
	}else{
	  //SDL_Log(("atan: "+toString(atan(fabs(y-outerMovementRing->pos.y)/fabs(x-outerMovementRing->pos.x)))).c_str());
	  //normalizing the vector
	  
	  float difX = x-outerMovementRing->pos.x;
	  float difY = y-outerMovementRing->pos.y;

	  float pathag = sqrt(pow(fabs(x-outerMovementRing->pos.x),2)+pow(fabs(y-outerMovementRing->pos.y),2));

	  float x = difX/pathag;
	  float y = difY/pathag;

	  SDL_Log(("THIS SHOULD BE 1: "+toString(fabs(x)+fabs(y))).c_str());

	  innerMovementCircle->pos.x = (x*0.2)+outerMovementRing->pos.x;
	  innerMovementCircle->pos.y = (y*0.2)+outerMovementRing->pos.y;
	}
	break;
    }
  }

}

void GameScreen::render(){

  glClear(GL_COLOR_BUFFER_BIT);


  mat::clearMatricies();

  mat::ortho(-screen::bounds[0],screen::bounds[0],screen::bounds[1],-screen::bounds[1],-1,1);

  mat::scale(1,1,1);
  /*mat::downloadMVP();

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glVertexAttribPointer(program::fixedColor::vertHandle,2,GL_FLOAT,false,0,0);

  glUniform4fv(program::fixedColor::colorHandle,1,&color[0]);

  glDrawArrays(GL_TRIANGLES,0,6);*/


  //SDL_Log("GL error: %s",gluErrorString(glGetError()));
  //
  //SDL_Log("GL error: %s",gluErrorString(glGetError()));
  //SDL_Log("GL error: %s",gluErrorString(glGetError()));


  /*glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glUniform4fv(program::fixedColor::colorHandle,1,&color[0]);

  glVertexAttribPointer(program::fixedColor::vertHandle,2,GL_FLOAT,false,0,0);

  glDrawArrays(GL_TRIANGLES,0,6);*/

  program::bind(program::textured::id);

  mat::downloadMVP();

  glEnableVertexAttribArray(program::textured::texCoordHandle);
  //SDL_Log("3 GL errors: %s",gluErrorString(glGetError()));

  //SDL_Log("8 GL errors: %s",gluErrorString(glGetError()));

  //SDL_Log("9 GL errors: %s",gluErrorString(glGetError()));

  glEnableVertexAttribArray(program::textured::vertHandle);



  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glUniform4fv(program::textured::colorHandle,1,&color[0]);

  glVertexAttribPointer(program::textured::vertHandle,2,GL_FLOAT,false,0,0);

  glBindBuffer(GL_ARRAY_BUFFER,texCoordBuf);

  float atexCoords[2*6] = {
    texCoords[0],texCoords[1],
    texCoords[0],texCoords[3],
    texCoords[2],texCoords[3],

    texCoords[0],texCoords[1],
    texCoords[2],texCoords[1],
    texCoords[2],texCoords[3]
  };

  glBufferData(GL_ARRAY_BUFFER,6*2*sizeof(float),&atexCoords[0],GL_STATIC_DRAW);
  glVertexAttribPointer(program::textured::texCoordHandle,2,GL_FLOAT,GL_FALSE,0,0);

  glActiveTexture(GL_TEXTURE0);
  //SDL_Log("10 GL errors: %s",gluErrorString(glGetError()));
  glBindTexture(GL_TEXTURE_2D, texture0);
  //SDL_Log("11 GL errors: %s",gluErrorString(glGetError()));
  glUniform1i(program::textured::textureUniform, 0);


  glDrawArrays(GL_TRIANGLES,0,6);

  
  glDisableVertexAttribArray(program::textured::texCoordHandle);

  glDisableVertexAttribArray(program::textured::vertHandle);

  program::bind(program::fixedColor::id);

  mat::downloadMVP();

  glEnableVertexAttribArray(program::fixedColor::vertHandle);

   body.draw();

  outerMovementRing->draw();

  innerMovementCircle->draw();

  glDisableVertexAttribArray(program::fixedColor::vertHandle);








}

void GameScreen::update(){

  keyInput();

  if(down){

    float x;
    float y;

    /*x = (event.tfinger.x*2)-1;
      y = (event.tfinger.y*(bounds[1]*2))-bounds[1];*/ //removed for computer compatibility

    int px, py;
    SDL_GetMouseState(&px, &py);

    x = ((px/float(screen::pixelBounds[0]))*2)-1;
    y = ((py/float(screen::pixelBounds[1]))*(screen::bounds[1]*2))-screen::bounds[1];

    float difX = x-outerMovementRing->pos.x;
    float difY = y-outerMovementRing->pos.y;

    float pathag = sqrt(pow(fabs(x-outerMovementRing->pos.x),2)+pow(fabs(y-outerMovementRing->pos.y),2));

    float incx = difX/pathag*0.002;
    float incy = difY/pathag*0.002;

    float a2 = a*2;

    texCoords[0]+=incx;
    if(texCoords[0]<0){
      texCoords[0] = 0;
      texCoords[2] = a2;
    }

    texCoords[1]+=incy;
    if(texCoords[1]<0){
      texCoords[1] = 0;
      texCoords[3] = a2;
    }

    texCoords[2]+=incx;
    if(texCoords[2]>1){
      texCoords[2] = 1;
      texCoords[0] = 1-a2;
    }

    texCoords[3]+=incy;
    if(texCoords[3]>1){
      texCoords[3] = 1;
      texCoords[1] = 1-a2;
    }

    for(int i=0;i<2*2;i+=2){

      SDL_Log(("texCoord "+toString(i)+" : "+toString(texCoords[i])+","+toString(texCoords[i+1])).c_str());

    }


    

  }


  render();

}
