#pragma once

#include <glm/glm.hpp>

#include "input.hpp"

class Camera
{
public:
	Camera()
	{
	}
	
	Camera( const glm::vec3& pos, const glm::vec3& dir );

	void update_keyboard( Input &input, float delta_time );
	void update_mouse( Input &input );

	inline void set_sensitivity( float s ) { m_sensitivity = s; }
	inline void set_position( const glm::vec3 &pos ) { m_position = pos; }

	constexpr glm::vec3 get_position()  const { return m_position;  }
	constexpr glm::vec3 get_direction() const { return m_direction; }
	constexpr glm::vec3 get_right()     const { return m_right;     }

	float &get_sensitivity() { return m_sensitivity; }

	glm::mat4 get_matrix() const;

private:
	glm::vec3 m_position  { 0.0f, 0.0f, 1.0f };
	glm::vec3 m_direction { 0.0f, 0.0f, -1.0f };
	glm::vec3 m_right     { 1.0f, 0.0f, 0.0f };

	double m_pitch = 0.0, m_yaw = 0.0;
	float m_sensitivity = 0.4f;

};