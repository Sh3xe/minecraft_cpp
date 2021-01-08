#ifndef RENDER_CONTEXT_INCLUDED_
#define RENDER_CONTEXT_INCLUDED_

#include <GLFW/glfw3.h>
#include "../config.hpp"

class RenderContext {
public:
	RenderContext( const Config &config );

	bool shouldClose();

private:
	GLFWwindow *m_window;
};

#endif