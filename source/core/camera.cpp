#include "camera.hpp"

#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include "input.hpp"


Camera::Camera( const glm::vec3& pos, const glm::vec3& dir ) :
	position(pos),
	direction(dir) {
}

void Camera::update_keyboard( float delta_time ) {

	constexpr float camera_speed { 2.0f };


	if ( Input::get().get_key(SDL_SCANCODE_W)) {
		position += glm::normalize(glm::vec3{ direction.x, 0.0f, direction.z }) * camera_speed * delta_time;
	}
	if ( Input::get().get_key(SDL_SCANCODE_S) ) {
		position -= glm::normalize(glm::vec3{ direction.x, 0.0f, direction.z }) * camera_speed * delta_time;
	}

	if ( Input::get().get_key(SDL_SCANCODE_A) ) {
		position += right * camera_speed * delta_time;
	}
	if ( Input::get().get_key(SDL_SCANCODE_D) ) {
		position -= right * camera_speed * delta_time;
	}

	if ( Input::get().get_key(SDL_SCANCODE_SPACE) ) {
		position += up *camera_speed * delta_time;
	}
	if ( Input::get().get_key(SDL_SCANCODE_LSHIFT) ) {
		position -= up *camera_speed * delta_time;
	}

}


void Camera::update_mouse() {

	float x = Input::get().get_mouse_x();
	float y = Input::get().get_mouse_y();

	static double last_x = x, last_y = y;

	yaw   += (x - last_x) * sensitivity;
	pitch += (last_y - y) * sensitivity; // invers�

	last_x = x; last_y = y;

	if (pitch > 89.0)
		pitch = 89.0;
	if (pitch < -89.0)
		pitch = -89.0;

	direction = {
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch))
	};

	right = glm::normalize( glm::cross({ 0.0f, 1.0f, 0.0f }, direction) );

}


glm::mat4 Camera::get_view() const
{
	return glm::lookAt(position, position + direction, { 0.0f, 1.0f, 0.0f });
}

glm::mat4 Camera::get_projection() const
{
	return glm::perspective ( fov, aspect_ratio, near, far );
}