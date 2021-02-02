#ifndef TEXTURE_INCLUDED_
#define TEXTURE_INCLUDED_

#include <string>
#include "../cmake_defines.hpp"

class Texture {
public:
	Texture( const std::string &path );
	~Texture();
	inline unsigned getId() const { return m_id; }

	// return true when the image is valid
	operator bool() const { return m_is_valid; };

private:
	unsigned m_id;
	bool m_is_valid;

};

#endif