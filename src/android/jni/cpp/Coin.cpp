// vim: sw=4 expandtab

#include "Coin.hpp"
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

static std::array<glm::vec3, 2> faceNormals = {{
	{ 0., 0., 1. },  // front face
	{ 0., 0., -1. }  // back face
}};

static GLuint glbufs[3];

constexpr int FACE_VERT_COUNT = 22;
constexpr int HEAD_IDX = 0;
constexpr int TAIL_START_IDX = FACE_VERT_COUNT;
constexpr int EDGE_START_IDX = TAIL_START_IDX + FACE_VERT_COUNT;
constexpr int EDGE_VERT_COUNT = 42;

namespace game
{
    float Coin::yAngle = 0.;
    float Coin::yTrans = 0.;
    float Coin::yDir = 0.;

    void Coin::genGraphics()
    {
        std::vector<glm::vec3> coinVertices;
        std::vector<glm::vec3> coinNormals;
        std::vector<uint16_t> coinIndices;

        coinVertices.reserve(FACE_VERT_COUNT*2);
        coinNormals.reserve(FACE_VERT_COUNT*2);
        coinIndices.reserve(FACE_VERT_COUNT*2);

        // front face
        coinVertices.push_back(glm::vec3{0.0,0.0,0.05});
        coinNormals.push_back(glm::vec3{0.0,0.0,1.0});
        // back face
        coinVertices.push_back(glm::vec3{0.0,0.0,-0.05});
        coinNormals.push_back(glm::vec3{0.0,0.0,-1.0});
        for (int i=0; i<FACE_VERT_COUNT-1; ++i)
        {
            float angle = i * M_PI * 0.1;
            float x = std::sin(angle);
            float y = std::cos(angle);
            auto norm = glm::vec3{x,y,0.};
            coinVertices.push_back(glm::vec3{x*0.5,y*0.5,0.05});
            coinNormals.push_back(norm);
            coinVertices.push_back(glm::vec3{x*0.5,y*0.5,-0.05});
            coinNormals.push_back(norm);
            //printf("%d: %.2f,%.2f\n", i, x, y);
        }

        coinIndices.push_back(0);
        for (int i=0; i<FACE_VERT_COUNT-1; ++i)
        {
            coinIndices.push_back(2+i*2);
        }
        assert(coinIndices.size() == TAIL_START_IDX);
        coinIndices.push_back(1);
        for (int i=0; i<FACE_VERT_COUNT-1; ++i)
        {
            coinIndices.push_back(3+i*2);
        }

        glGenBuffers(3,glbufs);
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[VERTBUF]);
        glBufferData(GL_ARRAY_BUFFER, coinVertices.size() * sizeof(glm::vec3), &coinVertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[NORMBUF]);
        glBufferData(GL_ARRAY_BUFFER, coinNormals.size() * sizeof(glm::vec3), &coinNormals[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glbufs[INDXBUF]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,coinIndices.size() * 2,&coinIndices[0],GL_STATIC_DRAW);
    }

    void Coin::draw()
    {
        float gold[] = {1.,0.843,0.,1.};
        auto prevModel = mat::model;
        mat::translate(pos[0]+0.5, pos[1]+0.5, pos[2]+0.5);
        auto vmmat = mat::view * mat::model;
        glm::vec3 lightPos = glm::vec3{vmmat * glm::vec4{2.,2.,2., 1.}};
        glm::vec3 light2Pos = glm::vec3{vmmat * glm::vec4{-2.,-2.,-2., 1.}};
        mat::translate(0., yTrans, 0.);
        mat::rotate(yAngle, 0., 1., 0);
        auto prog = static_cast<program::LightenFixedColor*>(program::getProgram(program::LIGHTEN_FIXED_COLOR));
        prog->bind();
        mat::downloadMVP();
        vmmat = mat::view * mat::model;
        glUniformMatrix4fv(prog->mvmUniform, 1, GL_FALSE, glm::value_ptr(vmmat));
        mat::model = prevModel;
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[VERTBUF]);
        glVertexAttribPointer(prog->vertHandle,3,GL_FLOAT,false,0,0);
        glEnableVertexAttribArray(prog->vertHandle);
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[NORMBUF]);
        glVertexAttribPointer(prog->normalHandle,3,GL_FLOAT,false,0,0);
        glEnableVertexAttribArray(prog->normalHandle);
        glUniform4fv(prog->colorHandle,1,gold);
        glUniform1f(prog->fogDensityHandle, 0.0);
        glUniform3fv(prog->lightPosUniform, 1, glm::value_ptr(lightPos));
        glUniform3fv(prog->light2PosUniform, 1, glm::value_ptr(light2Pos));
        glUniform1i(prog->doLightUniform, 1);
        glUniform1i(prog->doLight2Uniform, 1);
        glUniform1f(prog->shininessUniform, 1.);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,glbufs[INDXBUF]);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glUniform1i(prog->doUniformNormalsUniform, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 2, EDGE_VERT_COUNT);
        glUniform1i(prog->doUniformNormalsUniform, 1);
        for (int i=0; i<2; ++i)
        {
            glUniform3fv(prog->normalUniform, 1, &faceNormals[i][0]);
            glDrawElements(GL_TRIANGLE_FAN,FACE_VERT_COUNT,GL_UNSIGNED_SHORT,reinterpret_cast<void*>(i*2*FACE_VERT_COUNT));
            glFrontFace(GL_CCW);
        }
        glDisable(GL_CULL_FACE);
        glDisableVertexAttribArray(prog->vertHandle);
        glDisableVertexAttribArray(prog->normalHandle);
    }

    void Coin::update(float dt)
    {
        yAngle += dt * 2.;
        if (yAngle > 2.*M_PI)
            yAngle = 0.;
        yTrans += dt * yDir;
        if (yTrans <= 0.0)
        {
            yTrans = 0.;
            yDir = 1;
        }
        else if (yTrans > 0.5)
        {
            yTrans = 0.5;
            yDir = -1;
        }
    }
}

