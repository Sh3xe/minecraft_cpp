#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(){}
	Camera( const glm::vec3& pos, const glm::vec3& dir );

	void update_keyboard( float delta_time );
	void update_mouse();

	glm::mat4 get_matrix() const;

	glm::vec3 position  { 0.0f, 0.0f, 1.0f };
	glm::vec3 direction { 0.0f, 0.0f, -1.0f };
	glm::vec3 right     { 1.0f, 0.0f, 0.0f };
	glm::vec3 up        { 0.0f, 1.0f, 0.0f };

	float pitch = 0.0, yaw = 0.0;
	float sensitivity = 0.4f;
};