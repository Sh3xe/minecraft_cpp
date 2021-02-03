#ifndef CAMERA_INCLUDED_
#define CAMERA_INCLUDED_

#include <glm/glm.hpp>

class Input;

class Camera {
public:
	Camera() = default;
	Camera(const glm::vec3 &position , const glm::vec3& up );

	void setPosition( const glm::vec3 &position);
	void setUp( const glm::vec3 &up);
	void setSensitivity(double sensitivity);

	glm::vec3 getPosition() { return m_position; }
	glm::vec3 getDirection() { return m_direction; }

	void update(Input& input, double delta_time);
	glm::mat4 getViewMatrix();

private:
	glm::vec3 m_position;
	glm::vec3 m_up;
	glm::vec3 m_direction = glm::vec3(1.f, 0.f, 0.f);

	float m_pitch = 0.f, m_yaw = 0.f;
	double m_sensitivity = 0.005;

};

#endif