#ifndef PLAYER_INCLUDED_
#define PLAYER_INCLUDED_

#include <glm/glm.hpp>

#include "./core/camera.hpp"

class World;
class Input;

class Player {
public:
	Player( const glm::vec3& pos ):
		m_camera(pos, glm::vec3(0.f, 1.f, 0.f)),
		//m_aabb(pos.x, pos.y, pos.z, 0.8, 1.8, 0.8),
		m_pos(pos),
		m_vel(0.0f, 0.0f, 0.0f),
		m_on_ground(false) {
	}

	void update(Input &input, World& world, double delta_time);
	inline Camera& getCamera() {return m_camera;}

private:
	void checkCollision( const glm::vec3& vel, World& world );

	Camera m_camera;

	glm::vec3 m_pos;
	glm::vec3 m_vel;

	bool m_on_ground;
};

#endif