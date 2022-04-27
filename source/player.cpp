#include "player.hpp"

#include <vector>

#include "./world/world.hpp"
#include "core/input.hpp"
#include "./core/aabb.hpp"
#include <iostream>

void Player::update(Input &input, World& world, double delta_time) {

	m_camera.handleMouse(input, delta_time);

	glm::vec3 direction = m_camera.getDirection();
	direction.y = 0.0f;
	direction = glm::normalize(direction);

	if (input.is_key_pressed(SDL_SCANCODE_SPACE) && m_on_ground) {
		m_on_ground = false;
		m_vel.y = 0.08f;
	}


	if (input.is_key_pressed(SDL_SCANCODE_W)) {
		m_vel += direction * (static_cast<float>(delta_time) * 0.1f);
	}

	if (input.is_key_pressed(SDL_SCANCODE_S)) {
		m_vel -= direction * (static_cast<float>(delta_time) * 0.1f);
	}


	if (input.is_key_pressed(SDL_SCANCODE_D)) {
		m_vel += glm::cross(direction, m_camera.getUp()) * (static_cast<float>(delta_time) * 0.1f);
	}

	if (input.is_key_pressed(SDL_SCANCODE_A)) {
		m_vel -= glm::cross(direction, m_camera.getUp()) * (static_cast<float>(delta_time) * 0.1f);
	}

	check_collision({ m_vel.x, 0.0, 0.0 }, world);
	check_collision({ 0.0, m_vel.y, 0.0 }, world);
	check_collision({ 0.0, 0.0, m_vel.z }, world);

	m_camera.setPosition( m_pos + glm::vec3(0.0, 1.5, 0.0) );

	m_vel.y -= 0.1f * delta_time;
	m_vel.x *= 0.97f;
	m_vel.z *= 0.97f;
}

void Player::check_collision( const glm::vec3& vel, World& world ) {

	m_pos += vel;
	AABB aabb = AABB(m_pos.x - 0.4, m_pos.y, m_pos.z - 0.4, 0.8, 1.8, 0.8);
	std::vector<AABB> hitboxes = world.getHitBoxes( aabb );

	if (hitboxes.size() == 0) return;
	
	if (vel.y < 0) {
		m_on_ground = true;
		m_vel.y = 0;
	}
	else if (vel.y > 0) {
		m_vel.y = 0;
	}

	if (vel.x != 0) m_vel.x = 0;
	if (vel.z != 0) m_vel.z = 0;

	m_pos -= vel;

	/*
	for (auto& hitbox : hitboxes) {

		if (vel.x > 0.0f) {
			m_pos.x = hitbox.xmin - 0.4f;
			m_vel.x = 0.0f;
		}
		else if (vel.x < 0.0f) {
			m_pos.x = hitbox.xmax + 0.4f;
			m_vel.x = 0.0f;
		}

		if (vel.z > 0.0f) {
			m_pos.z = hitbox.zmin - 0.4f;
			m_vel.z = 0.0f;
		}
		else if (vel.z < 0.0f) {
			m_pos.z = hitbox.zmax + 0.4f;
			m_vel.z = 0.0f;
		}

		if (vel.y > 0.0f) {
			m_pos.y = hitbox.ymin - 1.8f;
			m_vel.y = 0.0f;
		}

		else if (vel.y < 0.0f) {
			m_pos.y = hitbox.ymax;
			m_vel.y = 0.0f;
			m_on_ground = true;
		}
	}*/
}
