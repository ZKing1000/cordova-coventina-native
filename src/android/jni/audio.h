#include <SDL_mixer.h>

#include <string>

typedef Mix_Music Music;

typedef Mix_Chunk Sound;

namespace audio{

  extern std::string dir;
  extern std::string musicDir;
  extern std::string soundDir;


  void init();

  Music* music(std::string path);

  Sound* sound(std::string path);

  void playMusic(Music* music);

  void playSound(Sound* sound);

};
