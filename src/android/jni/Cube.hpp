// vim: sw=4 expandtab
#ifndef CUBE_HPP_
#define CUBE_HPP_

#include "MeshItem.hpp"
#include <glm/vec3.hpp>
#include <cstdint>

namespace game
{
    class Cube : public MeshItem
    {
    public:
        static void genGraphics();
        void draw();
    };
}

#endif

