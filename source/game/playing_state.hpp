#pragma once

#include "base_state.hpp"
#include "world/world.hpp"
#include "game/player.hpp"
#include "core/input.hpp"

struct Settings;

class PlayingState: public State {
public:
	PlayingState( Settings &settings );
	virtual ~PlayingState();

	virtual void update( double delta_time ) override;
	virtual void render() override;

private:
	void on_click( MouseButton b );

	World m_world;
	Player m_player;
};