// vim: sw=4 expandtab

#include "../../screens/GameScreen.h"

#include <SDL.h>

#include "../../opengl/programs.h"

#include "../../matrix.h"

#include "../../loop.h"

#include "../../extra.h"

#include "../../read.h"

#include "Player.hpp"
#include "Floor.hpp"
#include "Cube.hpp"
#include "CubeMesh.hpp"
#if USE_ANTBAR == 1
#include "AntTweakBar.h"
#endif

using game::player;

constexpr float DEGRAD = M_PI / 180.;

game::CubeMesh cubeMesh;

Screen* GameScreen::instance = new GameScreen();

GameScreen::GameScreen(){

  loop::currentScreen = this;

}

static float colorVar[] = { 0., 1., 0. };

void initUI()
{
#if USE_ANTBAR == 1
    extern float avgFps;

    if (!TwInit(TW_OPENGL_CORE, NULL)) {
      SDL_Log("Failed to init TweakBar: %s\n", TwGetLastError());
      return;
    }

    int width = screen::pixelBounds[0];
    int height = screen::pixelBounds[1];
    TwWindowSize(width, height);
    TwBar *bar = TwNewBar("Stats");
    TwDefine(" Stats size='200 100' help='Is this helpful?\n' ");
    //TwAddVarRW(bar, "Color", TW_TYPE_COLOR3F, &colorVar, " opened=true help='color' ");
    TwAddVarRW(bar, "Framerate", TW_TYPE_FLOAT, &avgFps, " opened=true help='framerate' ");
#endif
}

void GameScreen::genGraphics(){

  initUI();

  if (0 /*2d stuff*/)
  {
    circle = shape::circle(1);

    circle->genGraphics();
  }

  game::Floor::genGraphics();
  game::Cube::genGraphics();

  scene = read::holden("WithNormals.holden");
  //scene = read::holden("GoodUVCoordsAndObjectCount.holden");

  shs = 0;
  if (true)
  {
    shs = new Shape();

    shs->verts = scene[0]->verts;
    shs->indices = scene[0]->indices;
    shs->colors = std::vector<ColorRGB>(1);
    shs->colors[0] = ColorRGB(1,0,0);

    SDL_Log(("Verts Size: "+toString(shs->verts.size())+" indices size: "+toString(shs->indices.size())).c_str());
  }

  for(int i=0;i<scene.size();i++){
    if(TexturedShape* shape = dynamic_cast<TexturedShape*>(scene[i])){
      shape->surface = new SimpleSurface("MapTexture.png");

      //for(int i=0;i<shape->verts.size();i++){
	//shape->verts[i].y+=96;
	//SDL_Log(("Vert "+std::to_string(i)+" : "+std::to_string(shape->verts[i].x)+","+std::to_string(shape->verts[i].y)+","+std::to_string(shape->verts[i].z)).c_str());
      //}

      SDL_Log(("vert num: "+toString(shape->verts.size())).c_str());
    }
  }

  cubeMesh.readCubes("map.in");
}

void GameScreen::keyInput(){

  SDL_Event event;

  while(SDL_PollEvent(&event)){

    if (TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION))
      continue;

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

      case SDL_KEYDOWN:
        switch( event.key.keysym.sym ){
          case SDLK_e:
            player.runForward();
            break;
          case SDLK_d:
            player.runBackward();
            break;
          case SDLK_f:
            player.runLeft();
            break;
          case SDLK_s:
            player.runRight();
            break;
          case SDLK_LEFT:
            player.setRotation(-1);
            break;
          case SDLK_RIGHT:
            player.setRotation(1);
            break;
          case SDLK_UP:
            player.setXRotation(-1);
            break;
          case SDLK_DOWN:
            player.setXRotation(1);
            break;
          case SDLK_SPACE:
            player.jump();
            break;
        }
	break;
      case SDL_KEYUP:
	switch( event.key.keysym.sym ){
          case SDLK_e:
          case SDLK_d:
            player.stopRunning();
            break;
          case SDLK_f:
          case SDLK_s:
            player.stopRunningSideways();
            break;
          case SDLK_LEFT:
          case SDLK_RIGHT:
            player.setRotation(0);
            break;
	  case SDLK_UP:
	  case SDLK_DOWN:
            player.setXRotation(0);
	    break;
          case SDLK_PAGEUP:
          case SDLK_PAGEDOWN:
            break;
	}
	break;

      case SDL_MOUSEBUTTONDOWN:
	{
	mouseButtonDown = true;


      }

      case SDL_MOUSEBUTTONUP:
	mouseButtonDown = false;
	break;
      case SDL_MOUSEMOTION:
	break;
    }
  }

}

void GameScreen::render(){

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  if (0 /*2d stuff*/)
  {
    mat::clearMatrices();
    mat::ortho(-screen::bounds[0],screen::bounds[0],screen::bounds[1],-screen::bounds[1],-1,1);

    program::bind(program::FIXED_COLOR);
    program::FixedColor* p2 = static_cast<program::FixedColor*>(program::getBound());
    glEnableVertexAttribArray(p2->vertHandle);
    mat::downloadMVP();

    //draw some 2d stuff

    circle->draw();
  }


  //3d time

  mat::clearMatrices();

  mat::perspective(60*DEGRAD,screen::pixelBounds[0]/float(screen::pixelBounds[1]));

  mat::view = player.viewMat;

  game::floor.draw();

  for (auto &cube : cubeMesh.cubes)
  {
    cube->draw();
  }

  // shapes

  mat::translate(0, 1, -8);
  mat::rotate(angle,0,1,0);
  mat::rotate(fabs(angle-(2*M_PI)),1,0,0);

  angle += 0.005;
  if(angle>=(2*M_PI))
    angle = 0;

  if (shs)
  {
    program::bind(program::FIXED_COLOR);

    program::FixedColor* p2 = static_cast<program::FixedColor*>(program::getBound());

    glEnableVertexAttribArray(p2->vertHandle);

    mat::downloadMVP();

    shs->drawMode = GL_LINE_LOOP;

    shs->draw();
  }

  offsetX+=0.005;
  offsetY+=0.005;

  if(offsetX>1)
    offsetX = 0;
  if(offsetY>1)
    offsetY = 0;

  if (false)
  {
    program::bind(program::TEXTURED_FIXED_COLOR);

    mat::downloadMVP();

    program::TexturedFixedColor* p = static_cast<program::TexturedFixedColor*>(program::getBound());

    glEnableVertexAttribArray(p->vertHandle);
    glEnableVertexAttribArray(p->texCoordHandle);
    //glEnableVertexAttribArray(p->normalHandle);

    glUniform2f(p->texCoordOffsetHandle,offsetX,offsetY);

    //glUniform3f(p->lightPosUniform,0,0,-3);

    for(int i=0;i<scene.size();i++){

      scene[i]->drawMode = GL_TRIANGLES;
      scene[i]->colors[0] = ColorRGB(0.75,0.75,0.75);
      scene[i]->draw();

      //SDL_Log(("Tex Coords size: "+std::to_string(scene[i]->texCoords.size())).c_str());

      scene[i]->drawMode = GL_LINE_LOOP;
      scene[i]->colors[0] = ColorRGB(1,0.5,1);
      scene[i]->draw();
    }
  }
  if (true)
  {
    program::bind(program::LIGHTEN_TEXTURED_FIXED_COLOR);

    mat::downloadMVP();

    program::LightenTexturedFixedColor* p = static_cast<program::LightenTexturedFixedColor*>(program::getBound());

    glEnableVertexAttribArray(p->vertHandle);
    glEnableVertexAttribArray(p->texCoordHandle);
    glEnableVertexAttribArray(p->normalHandle);

    glUniform2f(p->texCoordOffsetHandle,offsetX,offsetY);

    glUniform3f(p->lightPosUniform,0,0,-3);

    for(int i=0;i<scene.size();i++){

      scene[i]->drawMode = GL_TRIANGLES;
      scene[i]->colors[0] = ColorRGB(0.75,0.75,0.75);
      scene[i]->draw();

      //SDL_Log(("Tex Coords size: "+std::to_string(scene[i]->texCoords.size())).c_str());

      scene[i]->drawMode = GL_LINE_LOOP;
      scene[i]->colors[0] = ColorRGB(1,0.5,1);
      scene[i]->draw();
    }
  }

  TwDraw();
}

void GameScreen::update(float dt){

  keyInput();

  player.handleBlocks(cubeMesh, dt);
  player.update(dt);

  render();

}
