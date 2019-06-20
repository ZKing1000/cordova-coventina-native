// vim: sw=4 expandtab

#include "../../opengl/programs.h"

#include <SDL.h>

#include "../../extra.h"

namespace program{

  std::vector<Program*> programs;

  unsigned int bound;

  void init(){

    programs.push_back( new FixedColor() );
    programs.push_back( new TexturedFixedColor() );
    programs.push_back( new LightenTexturedFixedColor() );
    programs.push_back( new LightenFixedColor() );

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

  const GLchar* fogVertShaderSource =
      "uniform float fogDensity;\n"
      "vec4 mixFog(vec4 color, vec3 ecPosition) {\n"
      " vec3 fogColor = vec3(0.0, 0.0, 0.0);\n"
      " float fogFragCoord = abs(ecPosition.z);\n"
      " float fog = exp(-fogDensity * fogFragCoord);\n"
      " fog = clamp(fog, 0.0, 1.0);\n"
      " return vec4(mix(fogColor, color.rgb, fog), color.a);\n"
      "}";

    const GLchar* lightVertShaderSource =
        "uniform vec3 u_LightPos;\n"
        "uniform vec3 u_Light2Pos;\n"
        "uniform vec3 u_Normal;\n"
        "uniform float u_Shininess;\n"
        "uniform bool do_light;\n"
        "uniform bool do_light2;\n"
        "uniform bool do_uniformNormals;\n"
        "attribute vec3 a_Normal;\n"
        "vec3 doLight(vec3 color, vec3 ecPos, vec3 ecNormal, vec3 lightPos) {\n"
        "   vec3 lightVector = normalize(lightPos - ecPos);\n"
        "   float nDotL = dot(ecNormal, lightVector);\n"
        "   vec3 ambient = 0.1 * color;\n"
        "   vec3 diffuse = 0.6 * max(0., nDotL) * color;\n"
        "   vec3 specular = vec3(0.,0.,0.);\n"
        "   if (u_Shininess > 0.) {\n"
        "       vec3 reflection = (2.0 * ecNormal * nDotL) - lightVector;\n"
        "       vec3 viewDirection = -normalize(ecPos);\n"
        "       float rDotV = dot(reflection, viewDirection);\n"
        "       float specPower = 1.5;\n"
        "       specular = color * u_Shininess * pow(max(0.,rDotV), specPower);\n"
        "   }\n"
        "   return vec4(ambient + diffuse + specular, inColor.a);\n"
        "}\n"
        "vec4 addLight(vec4 inColor, mat4 MVM, vec4 vertPos) {\n"
        "   if (!do_light && !do_light2) return inColor;\n"
        "   vec4 modelViewVertex = MVM * vertPos;\n"
        "   vec3 ecPos = vec3(modelViewVertex);\n"
        "   vec3 ecNormal;\n"
        "   if (do_uniformNormals) {\n"
        "       ecNormal = vec3(MVM * vec4(u_Normal, 0.0));\n"
        "   }\n"
        "   else {\n"
        "       ecNormal = vec3(MVM * vec4(a_Normal, 0.0));\n"
        "   }\n"
        "   vec3 color = vec3(0.,0.,0.);\n"
        "   if (do_light)\n"
        "       color = color + doLight(inColor.rgb, ecPos, ecNormal, u_LightPos);\n"
        "   if (do_light2)\n"
        "       color = color + doLight(inColor.rgb, ecPos, ecNormal, u_Light2Pos);\n"
        "   return vec4(color, inColor.a);\n"
        "}";

  FixedColor::FixedColor(){

    const GLchar* vertShaderSource =
        "uniform mat4 MVP;\n"
        "uniform vec4 vColor;\n"
        "attribute vec4 vPosition;\n"
        "uniform float m_gl_PointSize;\n"
        "varying vec4 v_Color;\n"
        "void main() {\n"
            "gl_Position = MVP * vec4(vPosition[0],vPosition[1],vPosition[2],1);\n"
            "gl_PointSize = m_gl_PointSize;\n"
            "v_Color = mixFog(vColor, gl_Position.xyz);\n"
        "}\n";

    const GLchar* fragShaderSource =
        "varying vec4 v_Color;"
        "void main() {"
            "gl_FragColor = v_Color;"
        "}";

    const GLchar* vertShaderSources[2] = {program::fogVertShaderSource, vertShaderSource};
    defaultInit(vertShaderSources,2,fragShaderSource,FIXED_COLOR);
    fogInit();

    colorHandle = glGetUniformLocation(id,"vColor");

  }

  TexturedFixedColor::TexturedFixedColor(){

    const GLchar* tmpVertShader =
        "uniform mat4 MVP;"
        "attribute vec4 vPosition;"
        "uniform float m_gl_PointSize;"
        "attribute vec2 TexCoordIn;"
        "uniform bool doTexOffset;"
        "uniform vec2 TexCoordOffset;"
        "varying vec2 TexCoordOut;"
        "vec2 bridge;"
        "void main() {"
            "gl_Position = MVP * vec4(vPosition[0],vPosition[1],vPosition[2],1);"
            "gl_PointSize = m_gl_PointSize;"
            "bridge.x = TexCoordIn.x;"
            "bridge.y = TexCoordIn.y;"
            "if (doTexOffset) {"
            "   bridge.x += TexCoordOffset.x;"
            "   bridge.y += TexCoordOffset.y;"
            "   if(bridge.x > 1.){"
            "           bridge.x = TexCoordIn.x-1.;"
            "   }"
            "   if(bridge.y > 1.){"
            "           bridge.y = TexCoordIn.y-1.;"
            "   }"
            "}"
            "TexCoordOut = bridge;"
        "}";

    const GLchar* tmpFragShader =
        "uniform vec4 vColor;"
        "varying vec2 TexCoordOut;"
        "uniform sampler2D Texture;"
        "void main() {"
            "gl_FragColor =  texture2D(Texture, TexCoordOut);"
        "}";


    defaultInit(&tmpVertShader,1,tmpFragShader,TEXTURED_FIXED_COLOR);


    texCoordHandle = glGetAttribLocation(id, "TexCoordIn");

    textureUniform = glGetUniformLocation(id, "Texture");

    doTexOffsetHandle = glGetUniformLocation(id,"doTexOffset");
    texCoordOffsetHandle = glGetUniformLocation(id,"TexCoordOffset");


  }



  LightenTexturedFixedColor::LightenTexturedFixedColor(){

    const GLchar* tmpVertShader =
        "uniform mat4 MVP;\n"
        "uniform mat4 MVM;\n"
        "uniform float m_gl_PointSize;\n"
        "uniform bool doTexOffset;\n"
        "uniform vec2 TexCoordOffset;\n"
        "uniform vec4 vColor;\n"

        "attribute vec4 vPosition;\n"
        "attribute vec2 TexCoordIn;\n"

        "varying vec2 TexCoordOut;\n"
        "varying vec4 v_Color;\n"

        "vec2 bridge;\n"

        "void main() {\n"
            "v_Color = addLight(vec4(1,1,1,1), MVM, vPosition);\n"

            "gl_Position = MVP * vec4(vPosition[0],vPosition[1],vPosition[2],1);\n"
            "gl_PointSize = m_gl_PointSize;\n"

            "bridge.x = TexCoordIn.x;\n"
            "bridge.y = TexCoordIn.y;\n"
            "if (doTexOffset) {"
            "   bridge.x += TexCoordOffset.x;"
            "   bridge.y += TexCoordOffset.y;"
            "   if(bridge.x > 1.){"
            "           bridge.x = TexCoordIn.x-1.;"
            "   }"
            "   if(bridge.y > 1.){"
            "           bridge.y = TexCoordIn.y-1.;"
            "   }"
            "}"
            "TexCoordOut = bridge;\n"
        "}";

    const GLchar* tmpFragShader =
        "varying vec4 v_Color;"
        "varying vec2 TexCoordOut;"

        "uniform sampler2D Texture;"
        "void main() {"
            "gl_FragColor =  v_Color * texture2D(Texture, TexCoordOut);"
        "}";


    const GLchar* vertShaderSources[3] = {
        program::fogVertShaderSource,
        program::lightVertShaderSource,
        tmpVertShader
    };
    defaultInit(vertShaderSources,3,tmpFragShader,LIGHTEN_TEXTURED_FIXED_COLOR);
    lightInit(id);

    texCoordHandle = glGetAttribLocation(id, "TexCoordIn");

    textureUniform = glGetUniformLocation(id, "Texture");

    mvmUniform = glGetUniformLocation(id,"MVM");

    doTexOffsetHandle = glGetUniformLocation(id,"doTexOffset");
    texCoordOffsetHandle = glGetUniformLocation(id,"TexCoordOffset");

  }


  LightenFixedColor::LightenFixedColor(){

    const GLchar* tmpVertShader =
        "uniform mat4 MVP;\n"
        "uniform mat4 MVM;\n"
        "uniform float m_gl_PointSize;\n"
        "uniform vec4 vColor;\n"
        "attribute vec4 vPosition;\n"
        "varying vec4 v_Color;\n"
        "void main() {\n"
        "    v_Color = addLight(vColor, MVM, vPosition);\n"
        "    gl_Position = MVP * vec4(vPosition.xyz,1);\n"
        "    v_Color = mixFog(v_Color, gl_Position.xyz);\n"
        "    gl_PointSize = m_gl_PointSize;\n"
        "}";

    const GLchar* tmpFragShader =
        "varying vec4 v_Color;"
        "void main() {"
            "gl_FragColor = v_Color;"
        "}";

    const GLchar* vertShaderSources[3] = {
        program::fogVertShaderSource,
        program::lightVertShaderSource,
        tmpVertShader
    };

    defaultInit(vertShaderSources,3,tmpFragShader,LIGHTEN_FIXED_COLOR);
    fogInit();
    lightInit(id);

    colorHandle = glGetUniformLocation(id,"vColor");

    mvmUniform = glGetUniformLocation(id,"MVM");

  }

    void DummyLighten::lightInit(GLuint id)
    {
        normalHandle = glGetAttribLocation(id,"a_Normal");
        normalUniform = glGetUniformLocation(id,"u_Normal");
        shininessUniform = glGetUniformLocation(id,"u_Shininess");
        lightPosUniform = glGetUniformLocation(id,"u_LightPos");
        light2PosUniform = glGetUniformLocation(id,"u_Light2Pos");
        doLightUniform = glGetUniformLocation(id,"do_light");
        doLight2Uniform = glGetUniformLocation(id,"do_light2");
        doUniformNormalsUniform = glGetUniformLocation(id,"do_uniformNormals");
    }

}


Program::Program() :
    idx(program::programs.size())
{
}

void compileShader(GLuint shader)
{
    glCompileShader(shader);
    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &shaderCompiled );
    if( shaderCompiled != GL_TRUE ) {
        SDL_Log( "Unable to compile shader %d!\n", shader );
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
        char* infoLog = new char[ maxLength ];
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        SDL_Log( "Shader log: %s\n", infoLog );
        delete[] infoLog;
    }
}

void Program::defaultInit(const GLchar** vertSources, int vcnt, const GLchar* fragSource,program::EnumProgramIndices type){

  id = glCreateProgram();

  vertShader = glCreateShader(GL_VERTEX_SHADER);

  SDL_Log(("Shader id: "+toString(type)).c_str());
  glShaderSource(vertShader,vcnt,vertSources,nullptr);

  compileShader(vertShader);

  fragShader = glCreateShader(GL_FRAGMENT_SHADER);

  const GLchar* fragSources[1] = {fragSource};
  glShaderSource(fragShader,1,fragSources,nullptr);

  compileShader(fragShader);

  glAttachShader(id,vertShader);
  glAttachShader(id,fragShader);
  glLinkProgram(id);

  vertHandle = glGetAttribLocation(id,"vPosition");

}

void Program::fogInit()
{
    fogDensityHandle = glGetUniformLocation(id,"fogDensity");
}

void Program::bind()
{
    program::bound = idx;

    glUseProgram(id);
}

