#include "player.hpp"

#include <vector>

#include "./world/world.hpp"
#include "core/input.hpp"
#include <iostream>

void Player::update(Input &input, World& world, double delta_time) {
	m_camera.update(input, delta_time);

	auto cam_pos = m_camera.getPosition();
	m_aabb = AABB(cam_pos.x, cam_pos.y, cam_pos.z, 0.5, 0.5, 0.5);

	std::vector<AABB> hitboxes = world.getHitBoxes(this->m_aabb);

	std::cout << hitboxes.size() << '\n';

	for( AABB& box: hitboxes ) {
		// resolve collision
	}
}
