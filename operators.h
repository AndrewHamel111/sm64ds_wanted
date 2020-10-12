#ifndef OPERATORS_H_
#define OPERATORS_H_

bool operator==(target const& a, target const& b)
{
	return a.getX() == b.getX() && a.getY() == b.getY() && (a.isWanted() == b.isWanted());
}

#endif
