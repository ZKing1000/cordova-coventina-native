#ifndef GAME_GLCOMMON_H_
#define GAME_GLCOMMON_H_

#ifdef __ANDROID__
#define USE_GLES 1
#endif

#if USE_GLES == 1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#endif

