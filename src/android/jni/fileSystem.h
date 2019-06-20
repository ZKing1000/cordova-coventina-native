#include <SDL.h>

#include <string>

struct FakeStream {

  SDL_RWops* src;

  FakeStream(std::string path);

  bool read(void* start,unsigned size);

  bool getLine(std::string& line);

};

namespace fileSystem{

  std::string getAssetsPath();

}
