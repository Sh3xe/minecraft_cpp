#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <string>

class Shader {
public:
	Shader( const std::string &vs_path, const std::string &fs_path );
	//~Shader();

	inline unsigned getId() const { return m_id; }
	operator bool() const { return m_is_valid; }

	void use();

	// TODO: add setUniform[blabla]();
	void setInt( const std::string &name, int value );
	void setMat4( const std::string &name, float *value);

private:
	unsigned compileShader( const std::string &path, unsigned type );

	unsigned m_id;
	bool m_is_valid;
};

#endif