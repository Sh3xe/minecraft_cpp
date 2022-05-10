#pragma once

#include "base_state.hpp"
#include "world/world.hpp"
#include "player.hpp"
#include "core/input.hpp"

struct Config;

class PlayingState: public State {
public:
	PlayingState( Config &config );
	virtual ~PlayingState();

	virtual void update( double delta_time ) override;
	virtual void render() override;

private:
	void on_click( MouseButton b );

	World m_world;
	Player m_player;

};