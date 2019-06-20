// vim: sw=4 expandtab
#ifndef CUBEMESH_HPP_
#define CUBEMESH_HPP_

#include "Cube.hpp"
#include "Coin.hpp"
#include "Ring.hpp"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <vector>
#include <memory>
#include <cstdint>

namespace game
{
    constexpr int Collectible = 200;
    enum {
        WallBlockType = 1,
        WaterBlockType = 2,
        CoinType = 200,
        RingType = 201
    };

    class CubeMesh
    {
    public:
        const MeshItem &getNode(int x, int y, int z);
        void clearNode(const MeshItem &node);

        const MeshItem &getNode(const glm::ivec3 &pos)
        {
            return getNode(pos[0], pos[1], pos[2]);
        }
        uint8_t getType(int x, int y, int z)
        {
            return getNode(x, y, z).type;
        }
        uint8_t getType(const glm::ivec3 &pos)
        {
            return getNode(pos[0], pos[1], pos[2]).type;
        }
        uint8_t getIndex(int x, int y, int z)
        {
            return getNode(x, y, z).idx;
        }
        uint8_t getIndex(const glm::ivec3 &pos)
        {
            return getNode(pos[0], pos[1], pos[2]).idx;
        }
        void readMap(const char *filename);
        void readCubes(const char *filename);
        void addRandoms();
        void addCube(Cube cube);
        void addCoin(Coin coin);
        void addRing(Ring ring);

        std::vector<std::shared_ptr<Cube>> cubes;
        std::vector<std::shared_ptr<Coin>> coins;
        //std::vector<std::shared_ptr<WaterCube>> waterCubes;
        std::vector<std::shared_ptr<Ring>> rings;

        glm::ivec2 wellCenter;
        glm::ivec2 playerStart;
    };
}

#endif
