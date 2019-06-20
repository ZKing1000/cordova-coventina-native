// vim: expandtab sw=4
#include "Player.hpp"
#include "screen.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GLES2/gl2.h>
#include <cstdio>

#include "../screens/GameScreen.h"

constexpr float DEGTORAD = M_PI / 180.;
constexpr float PI_2 = M_PI * 2.;

constexpr float PlayerSpeed = 6.0;
constexpr float JumpSpeed = 5.0;
constexpr float HopSpeed = 1.0;
constexpr float RotationSpeed = 120.0 * DEGTORAD;
constexpr float GravityAccel = -9.0;
constexpr float PlayerHalfWidth = 0.4;
constexpr float PlayerHalfDepth = 0.3;
constexpr float PlayerHeight = 2.0;
constexpr float PlayerEyeLevel = 1.5;
const glm::vec3 PlayerBounds[2] = {
    { -PlayerHalfWidth, 0., -PlayerHalfDepth },
    { PlayerHalfWidth, PlayerHeight, PlayerHalfDepth }
};

enum {
    LeftKey = 0x1,
    RightKey = 0x2,
    ForwardKey = 0x4,
    BackwardKey = 0x8
};

uint8_t keys = 0;

extern game::CubeMesh cubeMesh;

namespace game
{
    Player player = {
        .pos = {0., 0., 3.},
        .yangle = 0.,
        .xangle = 0.,
        .zspeed = 0.0,
        .xspeed = 0.0,
        .yspeed = 0.0,
        .yaccel = 0.0,
        .inAir = false,
        .wallClimbing = false,
        .eyeVector = {0., 0., -1.},
        .xVector = {1., 0., 0.},
        .zVector = {0., 0., -1.},
        .velocity = {0., 0., 0.},
        .viewMat = glm::mat4{1.,0.,0.,0., 0.,1.,0.,0., 0.,0.,1.,0., 0.,-PlayerEyeLevel,0.,1.},
        .yAngularVelocity = 0.,
        .xAngularVelocity = 0.,
        .pointerMode = false,
        .isRotating = false,
        .zRunning = false,
        .xRunning = false,
        .ringCount = 0,
        .coinCount = 0,
        .score = 0
    };

    void Player::update(float dt)
    {
        static glm::vec3 lastPos = {0.,0.,0.};

        pos += velocity * dt;

        if (yAngularVelocity > 0.01 || yAngularVelocity < -0.01)
        {
            isRotating = true;
            yangle += yAngularVelocity * dt;
            if (yangle >= PI_2)
                yangle -= PI_2;
            else if (yangle < 0.0)
                yangle = PI_2 + yangle;
        }
        if (xAngularVelocity > 0.01 || xAngularVelocity < -0.01)
        {
            isRotating = true;
            xangle += xAngularVelocity * dt;
            if (xangle > M_PI_2)
                xangle = M_PI_2;
            else if (xangle < -M_PI_2)
                xangle = -M_PI_2;
        }

        if (isRotating || pos != lastPos)
        {
            isRotating = false;
            viewMat = glm::rotate(glm::mat4(1.0f), yangle, glm::vec3(0.,1.,0.));
            zVector[0] = viewMat[2][0];
            zVector[1] = 0;
            zVector[2] = -viewMat[2][2];
            xVector[0] = viewMat[0][0];
            xVector[1] = 0;
            xVector[2] = -viewMat[0][2];
            viewMat = glm::rotate(glm::mat4(1.0f), xangle, glm::vec3(1.,0.,0.)) * viewMat;

            eyeVector[0] = viewMat[2][0];
            eyeVector[1] = viewMat[2][1];
            eyeVector[2] = -viewMat[2][2];

            glm::vec3 trans = {-pos[0], -pos[1] - PlayerEyeLevel, -pos[2]};
            viewMat = glm::translate(viewMat, trans);
        }

        lastPos = pos;

        velocity = {0., yspeed, 0.};
        if ((!inAir) || zRunning)
            velocity += (zspeed * zVector);
        if ((!inAir) || xRunning)
            velocity += (xspeed * xVector);
        yspeed += yaccel * dt;
        if (yspeed > JumpSpeed)
        {
            yspeed = JumpSpeed;
        }
        else if (yspeed < -JumpSpeed)
        {
            yspeed = -JumpSpeed;
        }
    }

    //static int moveval = 0;

    void Player::runForward()
    {
        keys |= ForwardKey;
        player.zspeed = PlayerSpeed;
        if (!inAir)
        {
            player.zRunning = true;
        }
        //printf("forward %d r=%d\n", moveval++, player.zRunning);
    }

    void Player::runBackward()
    {
        keys |= BackwardKey;
        player.zspeed = -PlayerSpeed;
        if (!inAir)
        {
            player.zRunning = true;
        }
        //printf("backward %d r=%d\n", moveval++, player.zRunning);
    }

    void Player::stopForward()
    {
        keys &= ~ForwardKey;
        if (player.zspeed > 0)
        {
            //printf("zstop %d\n", moveval++);
            zspeed = 0.0;
            player.zRunning = false;
            if (keys & BackwardKey)
            {
                runBackward();
            }
        }
    }

    void Player::stopBackward()
    {
        keys &= ~BackwardKey;
        if (player.zspeed < 0)
        {
            //printf("zstop %d\n", moveval++);
            zspeed = 0.0;
            player.zRunning = false;
            if (keys & ForwardKey)
            {
                runForward();
            }
        }
    }

    void Player::runLeft()
    {
        keys |= LeftKey;
        player.xspeed = PlayerSpeed;
        if (!inAir)
        {
            player.xRunning = true;
        }
        //printf("left %d r=%d\n", moveval++, player.xRunning);
    }

    void Player::runRight()
    {
        keys |= RightKey;
        player.xspeed = -PlayerSpeed;
        if (!inAir)
        {
            player.xRunning = true;
        }
        //printf("right %d r=%d\n", moveval++, player.xRunning);
    }

    void Player::stopLeft()
    {
        keys &= ~LeftKey;
        if (xspeed > 0)
        {
            //printf("left stop %d\n", moveval++);
            xspeed = 0.0;
            player.xRunning = false;
            if (keys & RightKey)
            {
                runRight();
            }
        }
    }

    void Player::stopRight()
    {
        keys &= ~RightKey;
        if (xspeed < 0)
        {
            //printf("right stop %d\n", moveval++);
            xspeed = 0.0;
            player.xRunning = false;
            if (keys & LeftKey)
            {
                runLeft();
            }
        }
    }

    void Player::runDirection(Vec2 direction)
    {
        player.xspeed = direction.x * PlayerSpeed;
        player.zspeed = direction.y * PlayerSpeed;
        if (!inAir)
        {
            player.xRunning = true;
            player.zRunning = true;
        }
    }

    void Player::stopRunning()
    {
        //printf("stop %d\n", moveval++);
        xspeed = zspeed = 0.0;
        player.xRunning = false;
        player.zRunning = false;
    }

    void Player::setRotation(int dir)
    {
        player.yAngularVelocity = RotationSpeed * dir;
    }

    void Player::setXRotation(int dir)
    {
        player.xAngularVelocity = RotationSpeed * dir;
    }

    void Player::setPointerDelta(float x, float y)
    {
        if (pointerMode && player.yAngularVelocity == 0. && player.xAngularVelocity == 0.)
        {
            if (x != 0.)
            {
                float z2 = std::sqrt(1.0f - x*x);

                float cosAngle = z2;
                float qangle = std::acos(cosAngle) * 0.5;
                if (x < 0.)
                {
                    yangle -= qangle;
                    if (yangle < 0.0)
                        yangle = PI_2 + yangle;
                }
                else
                {
                    yangle += qangle;
                    if (yangle >= PI_2)
                        yangle -= PI_2;
                }

                isRotating = true;
            }

            if (y != 0.)
            {
                float z2 = std::sqrt(1.0f - y*y);

                float cosAngle = z2;
                float qangle = std::acos(cosAngle) * 0.5;
                if (y < 0.)
                {
                    xangle -= qangle;
                    if (xangle < -M_PI_2)
                        xangle = -M_PI_2;
                }
                else
                {
                    xangle += qangle;
                    if (xangle > M_PI_2)
                        xangle = M_PI_2;
                }

                isRotating = true;
            }
        }
    }

    void Player::togglePointerMode()
    {
        pointerMode = !pointerMode;
        if (pointerMode)
        {
            yAngularVelocity = 0.;
            xAngularVelocity = 0.;
        }
    }

    void Player::jump()
    {
        if (yspeed != 0.)
            return;
        yspeed = JumpSpeed;
        yaccel = GravityAccel;
        inAir = true;
        audio::playSound(GameScreen::jumpSound);
    }

    void Player::handleBlocks(CubeMesh &mesh, float dt)
    {
        auto nextpos = pos + velocity * dt;

        glm::ivec3 curmins = {
            floorf(pos[0]+PlayerBounds[0][0]),
            floorf(pos[1]+PlayerBounds[0][1]),
            floorf(pos[2]+PlayerBounds[0][2])
        };
        glm::ivec3 curmaxs = {
            floorf(pos[0]+PlayerBounds[1][0]),
            floorf(pos[1]+PlayerBounds[1][1]),
            floorf(pos[2]+PlayerBounds[1][2])
        };

        glm::ivec3 nextmins = {
            floorf(nextpos[0]+PlayerBounds[0][0]),
            floorf(nextpos[1]+PlayerBounds[0][1]),
            floorf(nextpos[2]+PlayerBounds[0][2])
        };
        glm::ivec3 nextmaxs = {
            floorf(nextpos[0]+PlayerBounds[1][0]),
            floorf(nextpos[1]+PlayerBounds[1][1]),
            floorf(nextpos[2]+PlayerBounds[1][2])
        };

        constexpr bool ladderEffect = false; // true for unlimited wall climbing
        bool isWallClimbing = false;
        bool onfloor = false;

        for (int y=nextmins[1]; y<=nextmaxs[1]; ++y)
        {
            for (int x=nextmins[0]; x<=nextmaxs[0]; ++x)
            {
                for (int z=nextmins[2]; z<=nextmaxs[2]; ++z)
                {
                    auto &node = mesh.getNode(x, y, z);
                    auto cubeType = node.type;
                    if (cubeType >= Collectible)
                    {
                        gotCollectible(node);
                        continue;
                    }
                    if (cubeType == 0)
                        continue;

                    //printf("collision pos=%.2f,%f,%.2f next=%.2f,%f,%.2f\n",
                    //        pos[0], pos[1], pos[2], nextpos[0], nextpos[1], nextpos[2]);

                    if (x < curmins[0])
                    {
                        //printf("-x %d,%d,%d\n", x,y,z);
                        velocity[0] = 0.;
                        if (zRunning && inAir && yspeed > 0)
                            isWallClimbing = true;
                        pos[0] = curmins[0] - PlayerBounds[0][0];
                    }
                    else if (x > curmaxs[0])
                    {
                        //printf("+x %d,%d,%d\n", x,y,z);
                        velocity[0] = 0.;
                        if (zRunning && inAir && yspeed > 0)
                            isWallClimbing = true;
                        pos[0] = curmaxs[0] + 0.99999 - PlayerBounds[1][0];
                    }
                    if (z < curmins[2])
                    {
                        static int val=0;
                        // hitting a solid block in front of us
                        //printf("-z %d,%d,%d\n", x, y, z);
                        velocity[2] = 0.;
                        if (zRunning && inAir && yspeed > 0)
                            isWallClimbing = true;
                        pos[2] = curmins[2] - PlayerBounds[0][2];
                    }
                    else if (z > curmaxs[2])
                    {
                        // hitting a solid block behind us
                        //printf("+z %d,%d,%d\n", x,y,z);
                        velocity[2] = 0.;
                        if (zRunning && inAir && yspeed > 0)
                            isWallClimbing = true;
                        pos[2] = curmaxs[2] + 0.99999 - PlayerBounds[1][2];
                    }
                    if (y < curmins[1])
                    {
                        // hitting a solid block beneath us
                        //printf("-y %d %d %d\n", y, curmins[1], nextmins[1]);
                        velocity[1] = 0.;
                        inAir = false;
                        yspeed = 0.;
                        yaccel = 0.;
                        pos[1] = curmins[1] - PlayerBounds[0][1];
                        nextpos[1] = pos[1];
                        nextmins[1] = curmins[1];
                    }
                    else if (y > curmaxs[1])
                    {
                        // hitting a solid block above us
                        //printf("+y %d\n", y);
                        velocity[1] = 0.;
                        yspeed = 0.;
                        pos[1] = curmaxs[1] + 0.99999 - PlayerBounds[1][1];
                        nextpos[1] = pos[1];
                        break;
                    }
                }
            }
        }

        if (isWallClimbing)
        {
            wallClimbing = true;
            //printf("wall climbing = true\n");
            if (ladderEffect)
                yaccel = 0.0;
        }
        else if (wallClimbing)
        {
            wallClimbing = false;
            //printf("wall climbing = false\n");
            if (yspeed > 0)
            {
                yaccel = GravityAccel;
                yspeed = HopSpeed;
            }
        }

        if (yspeed < 0. && nextpos[1] <= 0.)
        {
            //printf("ground\n");
            inAir = false;
            velocity[0] = yspeed = yaccel = 0;
            pos[1] = 0. - PlayerBounds[0][1];
        }
        else if (yaccel > GravityAccel && nextmins[1] > 0)
        {
            bool onBlock = false;
            int y = nextmins[1] - 1;
            for (int x=nextmins[0]; x<=nextmaxs[0]; ++x)
            {
                for (int z=nextmins[2]; z<=nextmaxs[2]; ++z)
                {
                    glm::ivec3 underFoot = {x, y, z};
                    auto &node = mesh.getNode(underFoot);
                    auto cubeType = node.type;
                    if (cubeType >= Collectible)
                    {
                        gotCollectible(node);
                        continue;
                    }
                    if (cubeType != 0)
                    {
                        onBlock = true;
                        if (cubeType == WaterBlockType && coinCount > 1 && ringCount > 0)
                        {
                            audio::playSound(GameScreen::dropSound);
                            ++score;
                            coinCount = 0;
                            ringCount = 0;
                        }
                        break;
                    }
                }
            }

            if (!onBlock)
            {
                //printf("falling %f %f %f\n", pos[0], pos[1], pos[2]);
                yaccel = GravityAccel;
                inAir = true;
            }
        }
    }

    void Player::gotCollectible(const MeshItem &node)
    {
        auto ctype = node.type;
        if (ctype == CoinType)
        {
            SDL_Log("got coin!");
            audio::playSound(GameScreen::pickupSound);
            cubeMesh.clearNode(node);
            ++coinCount;
        }
        else if (ctype == RingType)
        {
            SDL_Log("got ring!");
            audio::playSound(GameScreen::pickupSound);
            cubeMesh.clearNode(node);
            ++ringCount;
        }
        else
        {
            SDL_Log("whats this #%d?!", ctype);
        }
    }
}

