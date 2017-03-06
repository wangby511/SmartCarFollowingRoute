#ifndef COORDINATE_H
#define COORDINATE_H

#include<cmath>

class coordinate
{
public:
	int x, y;


	coordinate()
	{
		x = y = 0;
	}
	coordinate(int a)
	{
		x = y = a;
	}
	coordinate(int a, int b)
	{
		x = a;
		y = b;
	}
	double length()
	{
		double len;

		len = sqrt(x*x + y*y);

		return len;
	}
};

#endif