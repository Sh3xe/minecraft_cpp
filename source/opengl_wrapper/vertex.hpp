#ifndef VERTEX_INCLUDED_
#define VERTEX_INCLUDED_

struct Vertex {
	Vertex() = default;

	Vertex(int x, int y, int z, int tex_x, int tex_y, int light_value):
		x((unsigned char)x), y((unsigned char)y), z((unsigned char)z),
		tex_x((unsigned char)tex_x), tex_y((unsigned char)tex_y), light_value((unsigned char)light_value) {
	}

	unsigned char x, y, z, tex_x, tex_y, light_value;
};

#endif