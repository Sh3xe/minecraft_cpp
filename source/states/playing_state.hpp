#ifndef PLAYING_STATE_INCLUDED_
#define PLAYING_STATE_INCLUDED_

#include "base_state.hpp"
#include "../world/chunk.hpp"
#include "../core/camera.hpp"

class PlayingState: public State {
public:
	PlayingState();

	virtual void update( Input &input, double delta_time ) override;
	virtual void render() override;

private:
	Chunk m_chunk;
	Camera m_camera;

};

#endif