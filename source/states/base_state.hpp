#pragma once

class State {
public:
	State() {
	}

	virtual void update(double delta_time) = 0;
	virtual void render() = 0;

	inline bool should_close() const { return m_should_close; }

protected:
	bool m_should_close = false;
};