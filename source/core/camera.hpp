#pragma once

#include <glm/glm.hpp>

//TODO: make this a simple struct without data / move the movement code elsewhere

struct Camera
{
	Camera(){}
	Camera( const glm::vec3& pos, const glm::vec3& dir );

	void update_keyboard( float delta_time );
	void update_mouse();

	glm::mat4 get_view() const;
	glm::mat4 get_projection() const;

	glm::vec3 position  { 0.0f, 0.0f, 1.0f };
	glm::vec3 direction { 0.0f, 0.0f, -1.0f };
	glm::vec3 right     { 1.0f, 0.0f, 0.0f };
	glm::vec3 up        { 0.0f, 1.0f, 0.0f };

	float fov { 3.141592853f / 2.0f };
	float aspect_ratio {16.0f/9.0f};
	float near {0.01f};
	float far {250.0f};

	float pitch = 0.0, yaw = 0.0;
	float sensitivity = 0.4f;
};