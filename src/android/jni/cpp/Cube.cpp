// vim: sw=4 expandtab

#include "Cube.hpp"
#include "CubeMesh.hpp"
#include "matrix.h"
#include "opengl/programs.h"
#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <array>
#include <cstdint>
#include <cmath>

enum {
    VERTBUF,
    NORMBUF,
    INDXBUF
};

using namespace std;

static std::array<glm::vec3, 8> cubeVertices = {{
	{ 0., 0., 0. }, { 0., 1., 0.}, // 0 (left,  bottom, back),  1 (left, top, back)
	{ 1., 0., 0. }, { 1., 1., 0.}, // 2 (right, bottom, back),  3 (right, top, back)
	{ 1., 0., 1. }, { 1., 1., 1.}, // 4 (right, bottom, front), 5 (right, top, front)
	{ 0., 0., 1. }, { 0., 1., 1.}  // 6 (left,  bottom, front), 7 (left,  top, front)
}};

#if 0
static std::array<glm::vec3, 6*4> cubeNormals = {{
	{ -1., 0., 0. }, { -1., 0., 0. }, { -1., 0., 0. }, { -1., 0., 0. }, // left face
	{ 0., 0., 1. }, { 0., 0., 1. }, { 0., 0., 1. }, { 0., 0., 1. },     // front face
	{ 1., 0., 0. }, { 1., 0., 0. }, { 1., 0., 0. }, { 1., 0., 0. },     // right face
	{ 0., 0., -1. }, { 0., 0., -1. }, { 0., 0., -1. }, { 0., 0., -1. }, // back face
	{ 0., 1., 0. }, { 0., 1., 0. }, { 0., 1., 0. }, { 0., 1., 0. },     // top face
        { 0., -1., 0.}, { 0., -1., 0.}, { 0., -1., 0.}, { 0., -1., 0.}      // bottom face
}};
#endif
static std::array<glm::vec3, 6> cubeNormals = {{
	{ -1., 0., 0. },  // left face
	{ 0., 0., 1. },   // front face
	{ 1., 0., 0. },   // right face
	{ 0., 0., -1. },  // back face
	{ 0., 1., 0. },   // top face
        { 0., -1., 0.},   // bottom face
}};

enum {
	LBB, LTB,
	RBB, RTB,
	RBF, RTF,
	LBF, LTF
};

static std::array<uint16_t, 24> cubeIndices = {{
    LTB, LBB, LTF, LBF, // left face
    LTF, LBF, RTF, RBF, // front face
    RTF, RBF, RTB, RBB, // right face
    RTB, RBB, LTB, LBB, // back face
    RTB, LTB, RTF, LTF, // top face
    LBB, RBB, LBF, RBF  // bottom face

}};

static GLuint glbufs[3];

namespace game
{
    void Cube::genGraphics()
    {
        glGenBuffers(3,glbufs);
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[VERTBUF]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices[0], GL_STATIC_DRAW);
        //glBindBuffer(GL_ARRAY_BUFFER, glbufs[NORMBUF]);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), &cubeNormals[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glbufs[INDXBUF]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(cubeIndices),&cubeIndices[0],GL_STATIC_DRAW);
    }

    void Cube::draw()
    {
        float gray[] = {0.5,0.5,0.5,1.};
        //float green[] = {0.,1.,0.,1.};
        float blue[] = {0.0,0.5,1.0,1.};
        auto prevModel = mat::model;
        mat::translate(pos[0], pos[1], pos[2]);
        auto prog = static_cast<program::LightenFixedColor*>(program::getProgram(program::LIGHTEN_FIXED_COLOR));
        prog->bind();
        mat::downloadMVP();
        glUniformMatrix4fv(prog->mvmUniform, 1, GL_FALSE, glm::value_ptr(mat::view*mat::model));
        mat::model = prevModel;
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[VERTBUF]);
        glVertexAttribPointer(prog->vertHandle,3,GL_FLOAT,false,0,0);
        //glBindBuffer(GL_ARRAY_BUFFER, glbufs[NORMBUF]);
        if (type == WallBlockType)
            glUniform4fv(prog->colorHandle,1,gray);
        else
            glUniform4fv(prog->colorHandle,1,blue);
        glUniform1f(prog->fogDensityHandle, 0.02);
        auto lightPos = glm::vec3{mat::view * glm::vec4{10., 30., 20., 0.}};
        glUniform3fv(prog->lightPosUniform, 1, glm::value_ptr(lightPos));
        auto light2Pos = glm::vec3{mat::view * glm::vec4{-10., 30., -40., 0.}};
        glUniform3fv(prog->light2PosUniform, 1, glm::value_ptr(light2Pos));
        glUniform1i(prog->doLightUniform, 1);
        glUniform1i(prog->doLight2Uniform, 1);
        glUniform1i(prog->doUniformNormalsUniform, 1);
        glUniform1f(prog->shininessUniform, 0.);
        //glEnableVertexAttribArray(prog->normalHandle);
        glEnableVertexAttribArray(prog->vertHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glbufs[INDXBUF]);
        for (int i=0; i<6; ++i)
        {
            //glVertexAttribPointer(prog->normalHandle,3,GL_FLOAT,false,0,reinterpret_cast<void*>(0));
            glUniform3fv(prog->normalUniform, 1, &cubeNormals[i][0]);
            glDrawElements(GL_TRIANGLE_STRIP,4,GL_UNSIGNED_SHORT,reinterpret_cast<void*>(i*2*4));
        }
        glDisableVertexAttribArray(prog->vertHandle);
    }
}

