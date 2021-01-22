#ifndef PLAYING_STATE_INCLUDED_
#define PLAYING_STATE_INCLUDED_

#include "base_state.hpp"
#include "../opengl_wrapper/shader.hpp"
#include "../opengl_wrapper/texture.hpp"
#include "../core/camera.hpp"

class PlayingState: public State {
public:
	PlayingState();

	virtual void update( Input &input, double delta_time ) override;
	virtual void render() override;

private:
	/* World world; */
	// temporaire
	Shader m_shader;
	Texture m_texture;
	unsigned int m_vao, m_vbo;

	Camera m_camera;
};

#endif