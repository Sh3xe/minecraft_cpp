#ifndef PLAYING_STATE_INCLUDED_
#define PLAYING_STATE_INCLUDED_

#include "base_state.hpp"
#include "../opengl_wrapper/shader.hpp"

class PlayingState: public State {
public:
	PlayingState();

	virtual void update( Input &input, double delta_time ) override;
	virtual void render() override;

private:
	/* World world; */
	// temporaire
	unsigned int m_vao, m_vbo;
	Shader m_shader;
};

#endif