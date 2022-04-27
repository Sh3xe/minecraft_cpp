#pragma once

#include "base_state.hpp"
#include "../world/world.hpp"
#include "../player.hpp"

struct Config;

class PlayingState: public State {
public:
	PlayingState( Config &config );

	virtual void update( Input &input, double delta_time ) override;
	virtual void render() override;

private:
	World m_world;
	Player m_player;

};