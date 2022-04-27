#pragma once

#include <string>
#include <stdint.h>

class Shader {
public:
	Shader(const std::string& vs_path, const std::string& gs_path, const std::string& fs_path);
	~Shader();

	void bind();
	void unbind();

	operator bool() const { return m_is_valid; }

	void set_int( const std::string &name, int value );
	void set_float( const std::string& name, float value);
	void set_vec2( const std::string& name, float x, float y);
	void set_vec3( const std::string& name, float x, float y, float z);
	void set_vec4( const std::string& name, float x, float y, float z, float w);
	void set_mat4( const std::string& name, float* matrix );


private:
	uint32_t compile_shader( const std::string &path, uint32_t type);

	uint32_t m_id;
	bool m_is_valid = false;
};

