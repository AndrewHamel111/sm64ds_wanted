#ifndef OPERATORS_H_
#define OPERATORS_H_

bool operator==(target const& a, target const& b)
{
	return a.getX() == b.getX() && a.getY() == b.getY() && (a.isWanted() == b.isWanted());
}

bool operator<(const Vector2& v, const Rectangle& b)
{
	return (v.x > b.x && b.x + b.width > v.x) && (v.y > b.y && b.y + b.height > v.y);
}

#endif
