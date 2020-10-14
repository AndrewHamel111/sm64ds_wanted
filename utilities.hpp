#ifndef UTILITIES_H_
#define UTILITIES_H_

double hamAngle(double angle)
{
	while (angle > PI / 2)	angle -= PI / 2;
	return angle;
}

int hamSqr(int a)
{
	return a*a;
}

/// returns the square distance between two points (square because sqrt slow af)
int hamelDistance(float x1, float y1, float x2, float y2)
{
	return hamSqr(x2 - x1) + hamSqr(y2 - y1);
}

int hamelDistance(Vector2 one, Vector2 two)
{
	return hamelDistance(one.x, one.y, two.x, two.y);
}

int hamelDistance(Vector2 one, float twoX, float twoY)
{
	return hamelDistance(one.x, one.y, twoX, twoY);
}

int hamelDistanceCenter(Vector2 one, Vector2 two)
{
	return hamelDistance(one.x + 0.5*TARGET_HEIGHT, one.y + 0.5*TARGET_HEIGHT, two.x + 0.5*TARGET_HEIGHT, two.y + 0.5*TARGET_HEIGHT);
}

#endif

