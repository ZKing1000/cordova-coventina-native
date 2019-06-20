#include "../audio.h"

#include "../fileSystem.h"

#include <SDL.h>

namespace audio{

  std::string dir = "audio/";
  std::string musicDir = "music/";
  std::string soundDir = "sound/";


  void init(){

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
      SDL_Log("Could not initialize audio SDL_mixer error: %s",Mix_GetError());
      exit(1);
    }

  }

  Music* music(std::string path){

    return Mix_LoadMUS( (fileSystem::getAssetsPath()+dir+musicDir+path).c_str() );

  };

  Sound* sound(std::string path){

    return Mix_LoadWAV( (fileSystem::getAssetsPath()+dir+soundDir+path).c_str() );

  }

  void playMusic(Music* music){

    Mix_PlayMusic(music, 0 );

  }

  void playSound(Sound* sound){

    Mix_PlayChannel(-1, sound, 0 );

  }
  

};
