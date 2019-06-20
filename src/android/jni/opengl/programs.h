
#include "opengl.h"
#include <vector>

namespace program{
  enum EnumProgramIndices {
    FIXED_COLOR,
    TEXTURED_FIXED_COLOR,
    LIGHTEN_TEXTURED_FIXED_COLOR,
    LIGHTEN_FIXED_COLOR
  };
}

struct Program{

  GLuint id;
  GLuint colorHandle;
  GLuint vertHandle;
  GLuint fogDensityHandle;

  GLuint vertShader;
  GLuint fragShader;

  Program();

  void defaultInit(const GLchar** vertSources, int vcnt,const GLchar* fragSource,program::EnumProgramIndices);
  void fogInit();
  void bind();

private:
  unsigned idx;
};


namespace program{

  extern std::vector<Program*> programs;

  extern unsigned int bound;

  void init();

  void bind(EnumProgramIndices e);

  Program* getBound();
  Program* getProgram(EnumProgramIndices e);


  struct DummyTextured{

    GLuint texCoordHandle;
    GLuint textureUniform;
    GLuint doTexOffsetHandle;
    GLuint texCoordOffsetHandle;

  };

  struct DummyLighten{

    GLuint normalHandle;
    GLuint lightPosUniform;
    GLuint light2PosUniform;
    GLuint doLightUniform;
    GLuint doLight2Uniform;
    GLuint mvmUniform;
    GLuint normalUniform;
    GLuint doUniformNormalsUniform;
    GLuint shininessUniform;

    void lightInit(GLuint id);
  };



  struct FixedColor : public Program {

    FixedColor();

  };

  struct TexturedFixedColor : public DummyTextured, public Program {

    TexturedFixedColor();

  };

  struct LightenTexturedFixedColor : public DummyLighten , public DummyTextured, public Program {

    LightenTexturedFixedColor();

  };

  struct LightenFixedColor : public DummyLighten , public Program {

    LightenFixedColor();

  };

};
