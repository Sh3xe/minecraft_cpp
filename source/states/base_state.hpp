#ifndef BASE_STATE_INCLUDED_
#define BASE_STATE_INCLUDED_

#include <GLFW/glfw3.h>

class State {
public:
	virtual void update(float delta_time) = 0;
	virtual void render(float delta_time) = 0;

	GLFWwindow* window;
};

#endif