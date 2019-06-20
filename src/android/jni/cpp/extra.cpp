#include "../extra.h"
#include <sstream>

std::string toString(float in){
  std::ostringstream buff;
  buff<<in;
  return buff.str();
}
