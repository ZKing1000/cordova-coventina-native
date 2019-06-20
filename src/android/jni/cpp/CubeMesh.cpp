// vim: sw=4 expandtab

#include "CubeMesh.hpp"
#include "../fileSystem.h"
#include <SDL.h>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

extern unsigned WinScore;

constexpr int XD = 64;
constexpr int ZD = 64;
constexpr int YD = 8;

constexpr int XD2 = XD/2;
constexpr int ZD2 = ZD/2;
constexpr int YD2 = 1;

namespace game
{
    static vector<MeshItem*> cubemesh(XD*ZD*YD);

    static MeshItem noNode;

    const MeshItem &CubeMesh::getNode(int x, int y, int z)
    {
        if (y + YD2 < 0 || y + YD2 >= YD ||
                x + XD2 < 0 || x + XD2 >= XD ||
                z + ZD2 < 0 || z + ZD2 >= ZD)
            return noNode;
        int index = (y+YD2)*XD*ZD + (z+ZD2)*XD + (x+XD2);
        if (index < 0 || index >= cubemesh.size())
            return noNode;
        if (cubemesh[index] == nullptr)
            return noNode;
        return *cubemesh[index];
    }

    void CubeMesh::clearNode(const MeshItem &node)
    {
        auto &n = *cubemesh[node.midx];
        if (n.type == 0)
            return;
        MeshItem *item{nullptr};
        switch (n.type)
        {
        case CoinType:
            item = coins[node.idx].get();
            break;
        case RingType:
            item = rings[node.idx].get();
            break;
        default:
            item = cubes[node.idx].get();
            break;
        }
        item->valid = false;
        n.type = 0;
    }

    void CubeMesh::addCube(Cube cube)
    {
        auto &pos = cube.pos;
        auto ctype = cube.type;
        int index = (pos[1]+YD2)*XD*ZD + (pos[2]+ZD2)*XD + (pos[0]+XD2);
        while (cubemesh[index] != nullptr && cubemesh[index]->type > 0 && cubemesh[index]->type <= 2 && pos[1]+YD2 < YD)
        {
            ++pos[1];
            index = (pos[1]+YD2)*XD*ZD + (pos[2]+ZD2)*XD + (pos[0]+XD2);
        }
        if (cubemesh[index] != nullptr)
        {
            SDL_Log("overlap");
            return;
        }
        cubes.push_back(make_shared<Cube>(move(cube)));
        cubemesh[index] = cubes.back().get();
        cubemesh[index]->type = ctype;
        cubemesh[index]->idx = cubes.size() - 1;
        cubemesh[index]->midx = index;
    }

    void CubeMesh::addCoin(Coin coin)
    {
        auto &pos = coin.pos;
        int index = (pos[1]+YD2)*XD*ZD + (pos[2]+ZD2)*XD + (pos[0]+XD2);
        while (cubemesh[index] != nullptr && /*cubemesh[index]->type > 0 && cubemesh[index]->type <= 2 && */ pos[1]+YD2 < YD)
        {
            ++pos[1];
            index = (pos[1]+YD2)*XD*ZD + (pos[2]+ZD2)*XD + (pos[0]+XD2);
        }

        if (cubemesh[index] != nullptr)
        {
            SDL_Log("overlap");
            return;
        }
        coins.push_back(make_shared<Coin>(move(coin)));
        cubemesh[index] = coins.back().get();
        cubemesh[index]->type = CoinType;
        cubemesh[index]->idx = coins.size() - 1;
        cubemesh[index]->midx = index;
    }

    void CubeMesh::addRing(Ring ring)
    {
        auto &pos = ring.pos;
        int index = (pos[1]+YD2)*XD*ZD + (pos[2]+ZD2)*XD + (pos[0]+XD2);
        while (cubemesh[index] != nullptr && /*cubemesh[index]->type > 0 && cubemesh[index]->type <= 2 && */ pos[1]+YD2 < YD)
        {
            ++pos[1];
            index = (pos[1]+YD2)*XD*ZD + (pos[2]+ZD2)*XD + (pos[0]+XD2);
        }

        if (cubemesh[index] != nullptr)
        {
            SDL_Log("overlap");
            return;
        }
        rings.push_back(make_shared<Ring>(move(ring)));
        cubemesh[index] = rings.back().get();
        cubemesh[index]->type = RingType;
        cubemesh[index]->idx = rings.size() - 1;
        cubemesh[index]->midx = index;
    }

    void CubeMesh::readMap(const char *filename)
    {
        int z = -ZD2;
        int y = 0;

        wellCenter = glm::ivec2{0, -5};
        playerStart = glm::ivec2{0, 3};

        string line;
        std::string newFilename = (fileSystem::getAssetsPath()+std::string(filename));
        FakeStream f(newFilename);
        while (f.getLine(line))
        {
            int numx = int(line.size());
            for (int i=0; i<numx; ++i)
            {
                int x = i-XD2;

                if (line[i] == '*')
                {
                    playerStart = glm::ivec2{x,z};
                }
                else if (line[i] != ' ')
                {
                    Cube cube;
                    cube.pos = {x,y,z};
                    if (line[i] == '~')
                        cube.type = WaterBlockType;
                    else if (line[i] == '+')
                    {
                        wellCenter = glm::ivec2{x,z};
                        cube.type = WaterBlockType;
                    }
                    else if (line[i] == '=')
                    {
                        cube.type = WallBlockType;
                    }
                    else
                    {
                        cube.type = WallBlockType;
                        addCube(cube);
                        cube.pos[1] += 1;
                    }
                    addCube(move(cube));
                }
            }
            ++z;
        }
    }

    void CubeMesh::readCubes(const char *filename)
    {
        string line;
        std::string newFilename = (fileSystem::getAssetsPath()+std::string(filename));
        FakeStream f(newFilename);
        int linenum = 0;
        while (f.getLine(line))
        {
            ++linenum;
            auto p = line.find_first_not_of(" \t");
            if (p == string::npos)
                continue;
            else if (line[p] == '#')
                continue;
            int x, y, z, type;
            if (sscanf(line.c_str(), "%d %d %d %d", &x, &y, &z, &type) != 4)
            {
                cerr << filename << ": parse error on line " << linenum << ": " << line << endl;
                continue;
            }

            if (type == CoinType)
            {
                Coin coin;
                coin.pos = {x,y,z};
                addCoin(move(coin));
            }
            else if (type == RingType)
            {
                Ring ring;
                ring.pos = {x,y,z};
                addRing(move(ring));
            }
            else
            {
                Cube cube;
                cube.pos = {x,y,z};
                cube.type = type;
                addCube(move(cube));
            }
        }

    }
    void CubeMesh::addRandoms()
    {
        int nRandCoins = WinScore * 2 + 5;
        int nRandRings = WinScore + 3;
        int x, y, z;

#if 0
        for (int i=0; i<60; ++i)
        {
            do
            {
                x = float(rand()) * XD / RAND_MAX - XD2;
                y = 0;
                z = float(rand()) * ZD / RAND_MAX - ZD2;
            } while (std::abs(x) <= 7 && std::abs(z-1) <= 7);
            Cube cube;
            cube.pos = {x,y,z};
            cube.type = 1;
            addCube(move(cube));
        }
#endif

        for (int i=0; i<nRandCoins; ++i)
        {
            do
            {
                x = float(rand()) * XD / RAND_MAX - XD2;
                y = 0;
                z = float(rand()) * ZD / RAND_MAX - ZD2;
            } while (std::abs(x) <= 7 && std::abs(z-1) <= 7);
            Coin coin;
            coin.pos = {x,y,z};
            addCoin(move(coin));
        }
        for (int i=0; i<nRandRings; ++i)
        {
            do
            {
                x = float(rand()) * XD / RAND_MAX - XD2;
                y = 0;
                z = float(rand()) * ZD / RAND_MAX - ZD2;
            } while (std::abs(x) <= 7 && std::abs(z-1) <= 7);
            Ring ring;
            ring.pos = {x,y,z};
            addRing(move(ring));
        }
    }
}

