#pragma once
#include "math.h"

typedef struct magnititude
{
	float magnititude_value;
	float direction_x;
	float direction_y;
	float direction_z;
}magnititude;

typedef struct LH_pair
{
	int L;
	int H;
	int x_low;
	int y_low;
	int z_low;
	int x_high;
	int y_high;
	int z_high;
}LH_pair;

typedef struct point3D
{
	int x;
	int y;
	int z;
	point3D(int x_in, int y_in, int z_in)
	{
		x = x_in;
		y = y_in;
		z = z_in;
	}
	point3D()
	{
	}
	float Distance(float x_1, float y_1, float z_1)
	{
		return sqrt((x-x_1)*(x-x_1)+(y-y_1)*(y-y_1) + (z-z_1)*(z-z_1));
	}
	void point3D_minius(point3D p)
	{
		x = x-p.x;
		y = y-p.y;
		z = z-p.z;
	}
	bool IsSurface(float x_direct, float y_direct, float z_direct)
	{
		if(x ==0 && y == 0 && z == 0)
			return false;
		float a = x*x_direct + y*y_direct + z*z_direct;
		float cos_a = a/(sqrt(static_cast<float>(x*x + y*y + z*z)));
		if(abs(cos_a) > 0.50)
			return false;
		else
			 return true;
	}

	bool IsSurface2(float x_direct, float y_direct, float z_direct)
	{
		if(x ==0 && y == 0 && z == 0)
			return false;
		float a = x*x_direct + y*y_direct + z*z_direct;
		float cos_a = a/(sqrt(static_cast<float>(x*x + y*y + z*z)));
		if(abs(cos_a) > sqrt(2.0f)/2)
			return false;
		else
			return true;
	}
}point3D;
