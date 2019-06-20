// vim: sw=2 expandtab
#include "../read.h"

#include <SDL.h>

#include "../extra.h"

#include "../fileSystem.h"

std::vector<Shape*> read::holden(std::string path){

  path = fileSystem::getAssetsPath() + path;
  FakeStream fl(path);

  if(fl.src == nullptr){
    SDL_Log(("FILE "+path+" DOES NOT EXIST").c_str());
    exit(0);
  }

  char flTest[7];

  flTest[6] = 0; //null termination

  char test[] = "holden";

  fl.read(flTest,6);

  if(strcmp(flTest,test)){
    SDL_Log(("THIS WASNT A HOLDEN FILE: "+path).c_str());
    SDL_Log(("FILE HEADERS: "+std::string(flTest)+" AND: "+std::string(test)).c_str());
    SDL_Log(("header sizes: "+toString(sizeof(flTest))+" and "+toString(sizeof(test))).c_str());

    exit(0);
  }

  fl.read(test,1); //garbage

  unsigned short objectCount;
  fl.read(reinterpret_cast<char*>(&objectCount),2);

  SDL_Log(("Object Count: "+toString(objectCount)).c_str());

  std::vector<Shape*> shapes;

  for(int i=0;i<objectCount;i++){
    SDL_Log(std::string("FRESH").c_str());

    unsigned char type;

    fl.read(reinterpret_cast<char*>(&type),1); //type of objects

    SDL_Log(("TYPE: "+toString(type)).c_str());

    test[0] = 1;

    while(test[0] != 0){
      fl.read(test,1);
    }

    Shape* shape;

    if(type>=6)
      shape = new LightenTexturedShape();
    else if(type>=4)
      shape = new TexturedShape();
    else
      shape = new Shape();

    shapes.push_back(shape);

    unsigned short vertNum;

    fl.read(reinterpret_cast<char*>(&vertNum),2);

    SDL_Log(("SHAPE vert num: "+toString(vertNum)).c_str());

    shape->verts = std::vector<Vec>(vertNum);

    float fCache;

    for(int i=0;i<vertNum;i++){
      Vec* vec = &shape->verts[i];

      fl.read(reinterpret_cast<char*>(&fCache),4);
      vec->x = fCache;
      fl.read(reinterpret_cast<char*>(&fCache),4);
      vec->y = fCache;
      fl.read(reinterpret_cast<char*>(&fCache),4);
      vec->z = fCache;
    }

    unsigned short faceNum;
    fl.read(reinterpret_cast<char*>(&faceNum),2);

    SDL_Log(("face num: "+toString(faceNum)).c_str());

    bool useTris = (type%2);

    if(useTris)
      SDL_Log(("use tris: true"));
    else
      SDL_Log(("use tris: false"));

    int idxsPerFace = 6;
    if(useTris)
      idxsPerFace = 3;

    SDL_Log(("IDXS per face : "+toString(idxsPerFace)).c_str());

    shape->indices = std::vector<unsigned short>(faceNum*idxsPerFace);

    for(int i=0;i<faceNum*idxsPerFace;i+=idxsPerFace){
      unsigned short v0;
      unsigned short v1;
      unsigned short v2;
      unsigned short v3;

      fl.read(reinterpret_cast<char*>(&v0),2);
      fl.read(reinterpret_cast<char*>(&v1),2);
      fl.read(reinterpret_cast<char*>(&v2),2);
      if(!useTris)
	fl.read(reinterpret_cast<char*>(&v3),2);


      std::vector<unsigned short>& is = shape->indices;

      is[i] = v0;
      is[i+1] = v1;
      is[i+2] = v2;

      if(!useTris){
	is[i+3] = v2;
	is[i+4] = v0;
	is[i+5] = v3;
      }


    }


    if(type>=4){ //save uv coords

      unsigned short uvNum;

      fl.read(reinterpret_cast<char*>(&uvNum),2);

      SDL_Log(("uv num: "+toString(uvNum)).c_str());

      dynamic_cast<TexturedShape*>(shape)->texCoords = std::vector<Vec2>(uvNum);


      for(int i=0;i<uvNum;i++){
	Vec2 vec;

	fl.read(reinterpret_cast<char*>(&vec.x),4);
	fl.read(reinterpret_cast<char*>(&vec.y),4);

	dynamic_cast<TexturedShape*>(shape)->texCoords[i] = vec;
      }

    }


    if(type>=6){ //normals

      unsigned short normalNum = vertNum;

      LightenTexturedShape* tmp = dynamic_cast<LightenTexturedShape*>(shape);

      tmp->normals = std::vector<Vec>(normalNum);

      for(int i=0;i<normalNum;i++){

	Vec &vec = tmp->normals[i];

	fl.read(reinterpret_cast<char*>(&vec.x),4);
	fl.read(reinterpret_cast<char*>(&vec.y),4);
	fl.read(reinterpret_cast<char*>(&vec.z),4);

      }

    }
  }

  return shapes;

}
