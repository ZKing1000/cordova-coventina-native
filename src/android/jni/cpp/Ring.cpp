// vim: sw=4 expandtab

#include "Ring.hpp"
#include "read.h"
#include "opengl/programs.h"
#include "screens/GameScreen.h"
#include "matrix.h"
#include "Surface.h"

using namespace std;

namespace game
{
    std::vector<Shape*> Ring::shapes;

    float Ring::yAngle = 0.;
    float Ring::yTrans = 0.;
    float Ring::yDir = 0.;

    void Ring::genGraphics()
    {
        shapes = read::holden("Ring.holden");
        for (auto &shape : shapes)
        {
            shape->drawMode = GL_TRIANGLES;
            shape->colors[0] = ColorRGB(1.0,1.0,1.0);
            auto texshape = dynamic_cast<TexturedShape*>(shape);
            texshape->surface = new Surface("gold.png");
        }
    }

    void Ring::draw()
    {
        program::bind(program::LIGHTEN_TEXTURED_FIXED_COLOR);
        //program::bind(program::TEXTURED_FIXED_COLOR);

        auto prevModel = mat::model;
        mat::translate(pos[0]+0.5, pos[1]+0.5+yTrans, pos[2]+0.5);
        mat::rotate(yAngle,0.,1.,0.);
        mat::scale(0.4, 0.4, 0.4);

        program::LightenTexturedFixedColor* p = static_cast<program::LightenTexturedFixedColor*>(program::getBound());
        //program::TexturedFixedColor* p = static_cast<program::LightenTexturedFixedColor*>(program::getBound());

        glEnableVertexAttribArray(p->vertHandle);
        glEnableVertexAttribArray(p->texCoordHandle);
        glEnableVertexAttribArray(p->normalHandle);

        glUniform1i(p->doTexOffsetHandle,1);
        glUniform2f(p->texCoordOffsetHandle,GameScreen::instance->offsetX,GameScreen::instance->offsetY);
        glUniform1i(p->doLightUniform,0);
        glUniform1i(p->doLight2Uniform, 0);

        glUniform3f(p->lightPosUniform,1,1,1);

        for (auto &shape : shapes)
        {
            //shape->pos.x = pos[0];
            //shape->pos.y = pos[1];
            //shape->pos.z = pos[2];
            shape->draw();
        }

        mat::model = prevModel;
    }

    void Ring::update(float dt)
    {
        yAngle += dt * 3.;
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

