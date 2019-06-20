#ifndef _SURFACE_H
#define _SURFACE_H

#include <string>

struct Surface {
  int w;
  int h;
  unsigned char* data;
  unsigned components;

  Surface(std::string path);
};

#endif
