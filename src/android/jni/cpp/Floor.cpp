// vim: sw=4 expandtab

#include "Floor.hpp"
#include "Player.hpp"
#include "matrix.h"
#include "opengl/programs.h"
#include "glm/vec3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <cmath>

enum {
    VERTBUF,
    TEXCBUF,
    INDXBUF
};

using namespace std;

static GLuint glbufs[2];
static int nverts;
static int nindices;
GLuint texture;

Surface *surface;

namespace game
{
    Floor floor;

    void Floor::genGraphics()
    {
        constexpr int xdim = 80;
        constexpr int zdim = 40;

        nverts = xdim*zdim;
        vector<glm::vec3> verts(nverts);
        vector<glm::vec2> texcoords(nverts);

        constexpr float xbase = -40.;
        constexpr float zbase = -20.;
        constexpr float w = 1.;

        int index = 0;
        for (int z=0; z<zdim; ++z)
        {
            for (int x=0; x<xdim; ++x)
            {
                texcoords[index] = {x*w, z*w};
                verts[index++] = {xbase+x*w, 0, zbase+z*w};
            }
        }

        glGenBuffers(3,glbufs);
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[VERTBUF]);
        glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(verts[0]), glm::value_ptr(verts[0]), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[TEXCBUF]);
        glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(texcoords[0]), glm::value_ptr(texcoords[0]), GL_STATIC_DRAW);

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        surface = new Surface("grass.png");
        if (surface->components == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->data);
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glDisable(GL_TEXTURE_2D);

        int ntriangles = (xdim-1) * (zdim-1) * 2;
        nindices = ntriangles * 3;
        vector<uint16_t> indices(nindices);

        index = 0;
        for (int z=0; z<zdim-1; ++z)
        {
            for (int x=0; x<xdim-1; ++x)
            {
                int baseidx = z*xdim + x;
                int baseidx1 = (z+1)*xdim + x;
                indices[index++] = baseidx1;
                indices[index++] = baseidx + 1;
                indices[index++] = baseidx;
                indices[index++] = baseidx1 + 1;
                indices[index++] = baseidx + 1;
                indices[index++] = baseidx1;
            }
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glbufs[INDXBUF]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * 2, &indices[0], GL_STATIC_DRAW);
    }

    void Floor::draw()
    {
        float green[] = {0.05,0.7,0.05,1.};
        auto prevModel = mat::model;
        float rot = int((player.yangle+M_PI_4) * M_2_PI) * M_PI_2;
        float delta = (player.yangle - rot) * (1./M_PI_4);
        mat::translate(floorf(player.pos[0]), -0.01, floorf(player.pos[2]));
        mat::rotate(rot, 0., -1., 0.);
        mat::translate(floorf(delta * 15.), 0., -17.);
        auto prog = static_cast<program::TexturedFixedColor*>(program::getProgram(program::TEXTURED_FIXED_COLOR));
        prog->bind();
        mat::downloadMVP();
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[VERTBUF]);
        glVertexAttribPointer(prog->vertHandle,3,GL_FLOAT,false,0,0);
        glBindBuffer(GL_ARRAY_BUFFER, glbufs[TEXCBUF]);
        glVertexAttribPointer(prog->texCoordHandle,2,GL_FLOAT,false,0,0);
        glUniform4fv(prog->colorHandle,1,green);
        glUniform1f(prog->fogDensityHandle, 0.1);
        glEnableVertexAttribArray(prog->vertHandle);
        glEnableVertexAttribArray(prog->texCoordHandle);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(prog->textureUniform, 0);
        glUniform1i(prog->doTexOffsetHandle,1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glbufs[INDXBUF]);
        glDrawElements(GL_TRIANGLES, nindices, GL_UNSIGNED_SHORT, nullptr);
        //glDrawArrays(GL_LINES, 0, nverts);
        mat::model = prevModel;
    }
}

