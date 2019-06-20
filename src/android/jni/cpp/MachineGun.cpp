#include "../MachineGun.h"

#include "../Player.hpp"

#include "../loop.h"

#include "../matrix.h"

#include "../read.h"

float HeldMachineGun::timeTillNextFireInc;

HeldMachineGun::HeldMachineGun(){

  inc(&timeTillNextFireInc,1.);

}

void HeldMachineGun::keyInput(SDL_Event e){

  switch(e.type){
    case SDL_KEYDOWN:
      switch(e.key.keysym.sym){
	case SDLK_a:
	  firing = true;
	  break;
      }
      break;
    case SDL_KEYUP:
      switch(e.key.keysym.sym){
	case SDLK_a:
	  firing = false;
	  break;
      }
      break;
  }

}

void HeldMachineGun::update(){

  if(firing){

    if(timeTillNextFireCache >= 1.){

      fire();

      timeTillNextFireCache = 0;
    }

    timeTillNextFireCache += timeTillNextFireInc;

  }

}

void HeldMachineGun::draw(){



}

void HeldMachineGun::fire(){

  SDL_Log(("FIRE"));

  Bullet* bullet = new Bullet();

  bullet->acceleration = {game::player.eyeVector[0],game::player.eyeVector[1],game::player.eyeVector[2]};

  Vec* a = &bullet->acceleration;

  inc(&a->x,a->x);
  inc(&a->y,a->y);
  inc(&a->z,a->z);

  bullet->pos = {game::player.pos[0],game::player.pos[1],game::player.pos[2]};

  bullet->velocity = {a->x*2,a->y*2,a->z*2};

  Bullet::instances.push_back(bullet);

}

std::vector<Bullet*> Bullet::instances;

LightenTexturedShape* Bullet::shape;

void Bullet::update(){

  pos.x += velocity.x;
  pos.y += velocity.y;
  pos.z += velocity.z;

  velocity.x -= acceleration.x;
  velocity.y -= acceleration.y;
  velocity.z -= acceleration.z;

}

void Bullet::draw(){

  mat::translate(pos.x,pos.y,pos.z);

  shape->draw();

  mat::translate(-pos.x,-pos.y,-pos.z);

}

void Bullet::genGraphics(){

  shape = dynamic_cast<LightenTexturedShape*>(read::holden("Ring2.holden")[0]);

}
