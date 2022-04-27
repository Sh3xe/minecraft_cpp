#pragma once

#include "texture.hpp"

#include <initializer_list>
#include <string>

class CubemapTexture: public Texture {
public:
	CubemapTexture():
		Texture() {
	}

	CubemapTexture(
		const std::string &right,
		const std::string &left,
		const std::string &top,
		const std::string &bottom,
		const std::string &front,
		const std::string &back
	);

	CubemapTexture( const std::initializer_list<std::string> &paths );

	bool load_from_file( const std::string &right,
		const std::string &left,
		const std::string &top,
		const std::string &bottom,
		const std::string &front,
		const std::string &back
	);

	bool load_from_file( const std::initializer_list<std::string> &paths );

	void bind() const;
	void unbind() const;

};