
USE_GLES=1

BRYANFLAGS =
ifeq ($(shell id -un),bgreen)
BRYANFLAGS = -DBRYAN_
endif

ifeq ($(USE_GLES),0)
USE_ANTBAR = 1
else
USE_ANTBAR = 1
endif

CXXFLAGS += -std=gnu++11 -MMD -MP \
	    $(BRYANFLAGS) -DUSE_GLES=$(USE_GLES) -DUSE_ANTBAR=$(USE_ANTBAR) -I. \
	    $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_image SDL2_mixer glu)
CXXFLAGS += -g
#CXXFLAGS += -O2

LIBS =

ifeq ($(USE_ANTBAR),1)
TBARLIB = ui/AntTweakBar/lib/libAntTweakBar.a
LIBS += $(TBARLIB) -lX11
CPPFLAGS += -Iui/AntTweakBar/include
UIDEP = $(TBARLIB)
endif

LIBS += $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_image SDL2_mixer glu)

ifeq ($(USE_GLES),1)
LIBS += -lGLESv2
endif

SRCS = $(wildcard *.cpp cpp/*.cpp cpp/opengl/*.cpp cpp/screens/*.cpp cpp/ui/*.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

.PHONY: all game
all: $(UIDEP) game

game: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

ifeq ($(USE_ANTBAR),1)
$(UIDEP):
	$(MAKE) -C ui/AntTweakBar/src
endif

-include $(DEPS)

cpp/opengl/program.o: cpp/opengl/vert.glsl cpp/opengl/frag.glsl

.PHONY: clean
clean:
	rm -f $(OBJS) $(DEPS) game

