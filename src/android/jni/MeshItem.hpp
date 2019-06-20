// vim: sw=4 expandtab
#ifndef MESHITEM_HPP_
#define MESHITEM_HPP_

#include <glm/vec3.hpp>

namespace game
{
    class MeshItem
    {
    public:
        uint16_t idx;
        uint32_t midx;
        uint8_t type = 0;
        bool valid = true;
        glm::vec3 pos = {0., 0., 0.};

        int x() const { return pos[0]; }
        int y() const { return pos[1]; }
        int z() const { return pos[2]; }
    };
}

#endif

