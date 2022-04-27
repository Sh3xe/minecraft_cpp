#include "camera.hpp"

#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera( const glm::vec3& pos, const glm::vec3& dir ) :
	m_position(pos),
	m_direction(dir) {
}

void Camera::update_keyboard( Input &input, float delta_time ) {

	constexpr float camera_speed { 2.0f };


	if ( input.get_key(SDL_SCANCODE_W)) {
		m_position += glm::normalize(glm::vec3{ m_direction.x, 0.0f, m_direction.z }) * camera_speed * delta_time;
	}
	if ( input.get_key(SDL_SCANCODE_S) ) {
		m_position -= glm::normalize(glm::vec3{ m_direction.x, 0.0f, m_direction.z }) * camera_speed * delta_time;
	}

	if ( input.get_key(SDL_SCANCODE_A) ) {
		m_position += m_right * camera_speed * delta_time;
	}
	if ( input.get_key(SDL_SCANCODE_D) ) {
		m_position -= m_right * camera_speed * delta_time;
	}

	if ( input.get_key(SDL_SCANCODE_SPACE) ) {
		m_position += glm::vec3{ 0.0f, 1.0f, 0.0f } *camera_speed * delta_time;
	}
	if ( input.get_key(SDL_SCANCODE_LSHIFT) ) {
		m_position -= glm::vec3{ 0.0f, 1.0f, 0.0f } *camera_speed * delta_time;
	}

}


void Camera::update_mouse( Input &input ) {

	float x = input.get_mouse_x();
	float y = input.get_mouse_y();

	static double last_x = x, last_y = y;

	m_yaw   += (x - last_x) * m_sensitivity;
	m_pitch += (last_y - y) * m_sensitivity; // invers�

	last_x = x; last_y = y;

	if (m_pitch > 89.0)
		m_pitch = 89.0;
	if (m_pitch < -89.0)
		m_pitch = -89.0;

	m_direction = {
		cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)),
		sin(glm::radians(m_pitch)),
		sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch))
	};

	m_right = glm::normalize( glm::cross({ 0.0f, 1.0f, 0.0f }, m_direction) );

}


glm::mat4 Camera::get_matrix() const {
	return glm::lookAt(m_position, m_position + m_direction, { 0.0f, 1.0f, 0.0f });
}