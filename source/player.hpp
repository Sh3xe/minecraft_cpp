#ifndef PLAYER_INCLUDED_
#define PLAYER_INCLUDED_

#include <glm/glm.hpp>

#include "./core/camera.hpp"
#include "./core/aabb.hpp"

class World;
class Input;

class Player {
public:
	Player( const glm::vec3& pos ):
		m_camera(pos, glm::vec3(0.f, 1.f, 0.f)),
		m_aabb(pos.x, pos.y, pos.z, 0.5, 0.5, 0.5) {
	}

	void update(Input &input, World& world, double delta_time);
	inline Camera& getCamera() {return m_camera;}

private:
	Camera m_camera;
	AABB m_aabb;
};

#endif