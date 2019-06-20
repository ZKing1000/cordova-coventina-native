#include "../fileSystem.h"

FakeStream::FakeStream(std::string path){

  src = SDL_RWFromFile(path.c_str(), "rb");

  if(src == nullptr){
    SDL_Log(("FAKE STREAM CANNOT READ FILE: "+path).c_str());
    exit(0);
  }

}

bool FakeStream::read(void* start,unsigned size){

  return SDL_RWread(src,start,size,1);

}

bool FakeStream::getLine(std::string& line){

  line.clear();

  char tmp[1];

  while(read(&tmp[0],1)){
    if(tmp[0] == '\n')
      return true;

    line+=tmp[0];
  }

  return false;

}

std::string fileSystem::getAssetsPath()
{
  std::string path;
#ifndef __ANDROID__
#ifdef BRYAN_
  char cwdbuf[512];
  std::string home = getenv("PWD");
  path = home + "/assets/";
#else
  std::string home = getenv("HOME");
  path = home + "/Desktop/Programming/c++/android/SDL2-2.0.3/android-project/assets/";
#endif
#endif
  return path;
}

