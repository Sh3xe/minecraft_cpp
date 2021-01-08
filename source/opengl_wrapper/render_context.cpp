#include "render_context.hpp"

RenderContext::RenderContext( const Config &config ) {
	m_window = glfwCreateWindow(
		config.window_width,
		config.window_height,
		config.window_title,
		nullptr, nullptr
	);
}

bool RenderContext::shouldClose() {
	return glfwWindowShouldClose( m_window );
}