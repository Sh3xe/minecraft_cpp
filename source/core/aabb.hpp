#pragma once

struct AABB
{
	double xmin, xmax, ymin, ymax, zmin, zmax;

	AABB(double x, double y, double z, double w, double h, double l):
		xmin(x), xmax(x + w), ymin(y), ymax(y + h), zmin(z), zmax(z + l)
	{	
	}

	inline double get_width() { return xmax - xmin; };
	inline double get_height() { return ymax - ymin; };
	inline double get_depth() { return zmax - zmin; };

	inline bool collide_width(const AABB& other) const
	{
		return
			this->xmin >= other.xmax && this->xmax >= other.xmin &&
			this->ymin >= other.ymax && this->ymax >= other.ymin &&
			this->zmin >= other.zmax && this->zmax >= other.zmin;
	}

};