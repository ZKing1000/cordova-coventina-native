#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include "CubeMesh.hpp"
#include "opengl/opengl.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include <cstdint>

namespace game
{
	class Player
	{
	public:
		glm::vec3 pos;
		float yangle;
		float xangle;
		float zspeed;
		float xspeed;
		float yspeed;
		float yaccel;
                bool inAir;
                bool wallClimbing;
		glm::vec3 eyeVector;
		glm::vec3 xVector;
		glm::vec3 zVector;
		glm::vec3 velocity;
		glm::mat4 viewMat;
		float yAngularVelocity;
		float xAngularVelocity;
		bool pointerMode;
		bool isRotating;
		bool zRunning, xRunning;
		uint32_t ringCount, coinCount;
		unsigned score;

		void update(float dt);
		void runForward();
		void runBackward();
		void stopForward();
		void stopBackward();
		void runLeft();
		void runRight();
		void stopLeft();
		void stopRight();
		void runDirection(Vec2 direction);
		void stopRunning();
		void setRotation(int dir);
		void setXRotation(int dir);
		void setPointerDelta(float x, float y);
		void togglePointerMode();
		void jump();

                void handleBlocks(CubeMesh &mesh, float dt);
		void gotCollectible(const MeshItem &node);
	};

	extern Player player;
}

#endif

