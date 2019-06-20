// vim: sw=4 expandtab

#include "../../opengl/programs.h"

#include <SDL.h>

namespace program{

  std::vector<Program*> programs;

  unsigned int bound;

  void init(){

    programs.push_back( new FixedColor() );
    programs.push_back( new TexturedFixedColor() );
    programs.push_back( new LightenTexturedFixedColor() );

  }

  void bind(EnumProgramIndices e){
    programs[e]->bind();
  }

  Program* getBound(){
    return programs[bound];
  }

  Program* getProgram(EnumProgramIndices e)
  {
    return programs[e];
  }


  FixedColor::FixedColor(){

    const GLchar* vertShaderSource = R"EOF(
        uniform mat4 MVP;
        attribute vec4 vPosition;
        uniform float m_gl_PointSize;
        void main() {
            gl_Position = MVP * vec4(vPosition[0],vPosition[1],vPosition[2],1);
            gl_PointSize = m_gl_PointSize;
        }
)EOF";

    const GLchar* fragShaderSource = R"EOF(
        uniform vec4 vColor;
        void main() {
            gl_FragColor = vColor;
        }
)EOF";

    defaultInit(vertShaderSource,fragShaderSource);

    colorHandle = glGetUniformLocation(id,"vColor");

  }

  TexturedFixedColor::TexturedFixedColor(){

    const GLchar* tmpVertShader = R"EOF(
        uniform mat4 MVP;
        attribute vec4 vPosition;
        uniform float m_gl_PointSize;
        attribute vec2 TexCoordIn;
        uniform vec2 TexCoordOffset;
        varying vec2 TexCoordOut;
        vec2 bridge;
        void main() {
            gl_Position = MVP * vec4(vPosition[0],vPosition[1],vPosition[2],1);
            gl_PointSize = m_gl_PointSize;
            bridge.x = TexCoordIn.x;
            bridge.y = TexCoordIn.y;
            bridge.x += TexCoordOffset.x;
            bridge.y += TexCoordOffset.y;
            if(bridge.x > 1.){
                bridge.x = TexCoordIn.x-1.;
            }
            if(bridge.y > 1.){
                bridge.y = TexCoordIn.y-1.;
            }
            TexCoordOut = bridge;
        }
)EOF";

    const GLchar* tmpFragShader = R"EOF(
        uniform vec4 vColor;
        varying vec2 TexCoordOut;
        uniform sampler2D Texture;
        void main() {
            gl_FragColor =  texture2D(Texture, TexCoordOut);
        }
)EOF";


    defaultInit(tmpVertShader,tmpFragShader);


    texCoordHandle = glGetAttribLocation(id, "TexCoordIn");

    textureUniform = glGetUniformLocation(id, "Texture");

    texCoordOffsetHandle = glGetUniformLocation(id,"TexCoordOffset");


  }



  LightenTexturedFixedColor::LightenTexturedFixedColor(){

    const GLchar* tmpVertShader = R"EOF(
        uniform mat4 MVP;
        uniform mat4 MVM;
        uniform float m_gl_PointSize;
        uniform vec2 TexCoordOffset;
        uniform vec3 u_LightPos;

        attribute vec4 vPosition;
        attribute vec2 TexCoordIn;
        attribute vec3 a_Normal;
        //attribute vec4 a_Color;

        varying vec2 TexCoordOut;
        varying vec4 v_Color;

        vec2 bridge;

        void main() {
            vec3 modelViewVertex = vec3(MVM * vPosition);
            vec3 modelViewNormal = vec3(MVM * vec4(a_Normal, 0.0));
            float distance = length(u_LightPos - modelViewVertex);
            vec3 lightVector = normalize(u_LightPos - modelViewVertex);
            float diffuse = max(dot(modelViewNormal, lightVector), 0.1);
            diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));
            v_Color =/* a_Color **/ diffuse;

            gl_Position = MVP * vec4(vPosition[0],vPosition[1],vPosition[2],1);
            gl_PointSize = m_gl_PointSize;

            bridge.x = TexCoordIn.x;
            bridge.y = TexCoordIn.y;
            bridge.x += TexCoordOffset.x;
            bridge.y += TexCoordOffset.y;
            if(bridge.x > 1.){
                bridge.x = TexCoordIn.x-1.;
            }
            if(bridge.y > 1.){
                bridge.y = TexCoordIn.y-1.;
            }
            TexCoordOut = bridge;
        }
)EOF";

    const GLchar* tmpFragShader = R"EOF(
        uniform vec4 vColor;

        varying vec4 v_Color;
        varying vec2 TexCoordOut;

        uniform sampler2D Texture;
        void main() {
            gl_FragColor =  v_Color * texture2D(Texture, TexCoordOut);
        };
)EOF";


    defaultInit(tmpVertShader,tmpFragShader);

    normalHandle = glGetAttribLocation(id,"a_Normal");


    texCoordHandle = glGetAttribLocation(id, "TexCoordIn");

    textureUniform = glGetUniformLocation(id, "Texture");

    lightPosUniform = glGetUniformLocation(id,"u_LightPos");

    mvmUniform = glGetUniformLocation(id,"MVM");

    texCoordOffsetHandle = glGetUniformLocation(id,"TexCoordOffset");

  }





};


Program::Program() :
    idx(program::programs.size())
{
}

void Program::defaultInit(const GLchar* vertSource,const GLchar* fragSource){

  id = glCreateProgram();

  vertShader = glCreateShader(GL_VERTEX_SHADER);

  //SDL_Log(("Vert Shader: "+std::string(vertSource)).c_str());

  const GLchar* vertSources[1] = {vertSource};

  glShaderSource(vertShader,1,vertSources,nullptr);

  glCompileShader(vertShader);

  GLint vShaderCompiled = GL_FALSE;
  glGetShaderiv( vertShader, GL_COMPILE_STATUS, &vShaderCompiled );
  if( vShaderCompiled != GL_TRUE ) {
    SDL_Log( "Unable to compile vertex shader %d!\n", vertShader );

    int infoLogLength = 0;
    int maxLength = infoLogLength;
    glGetShaderiv( vertShader, GL_INFO_LOG_LENGTH, &maxLength );
    char* infoLog = new char[ maxLength ];
    glGetShaderInfoLog( vertShader, maxLength, &infoLogLength, infoLog );
    SDL_Log( "Shader log: %s\n", infoLog );
    delete[] infoLog;
  }

  fragShader = glCreateShader(GL_FRAGMENT_SHADER);

  const GLchar* fragSources[1] = {fragSource};
  glShaderSource(fragShader,1,fragSources,nullptr);

  glCompileShader(fragShader);

  GLint fShaderCompiled = GL_FALSE;
  glGetShaderiv( fragShader, GL_COMPILE_STATUS, &fShaderCompiled );
  if( fShaderCompiled != GL_TRUE ) {
    SDL_Log( "Unable to compile fragment shader %d!\n", fragShader );

    int infoLogLength = 0;
    int maxLength = infoLogLength;
    glGetShaderiv( fragShader, GL_INFO_LOG_LENGTH, &maxLength );
    char* infoLog = new char[ maxLength ];
    glGetShaderInfoLog( fragShader, maxLength, &infoLogLength, infoLog );
    SDL_Log( "Shader log: %s\n", infoLog );
    delete[] infoLog;
  }

  glAttachShader(id,vertShader);
  glAttachShader(id,fragShader);
  glLinkProgram(id);

  vertHandle = glGetAttribLocation(id,"vPosition");

}

void Program::bind()
{
    program::bound = idx;

    glUseProgram(id);
}

