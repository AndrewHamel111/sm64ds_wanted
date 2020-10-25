#ifndef OPERATORS_H_
#define OPERATORS_H_

////////////////////
// Enhanced std:: //
////////////////////

// output vector of strings
std::ostream& operator<<(std::ostream& os, std::vector<std::string>& v)
{
	if (v.empty()) return os << "{ VECTOR_EMPTY }";

	auto iter = v.begin();
	auto iEnd = v.end() - 1;
	os << "{ ";
	while(iter != iEnd) os << *(iter++) << " ";

	return os << "}";
}

// basic linear search algorithms
bool is_in(std::vector<std::string> names, std::string str)
{
	auto iter = names.begin(), iEnd = names.end();
	bool q = false;
	while (iter != iEnd) q |= (str == *(iter++));

	return q;
}

bool not_in(std::vector<std::string> names, std::string str)
{
	return !is_in(names, str);
}

////////////////////////////
// Raylib type extensions //
////////////////////////////

// Vector addition
Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return Vector2{a.x + b.x, a.y + b.y};
}

// Vector2& operator+=(const Vector2& rhs)
// {
	// this->x += rhs.x;
	// this->y += rhs.y;
	// return *this;
// }

Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return Vector2{a.x - b.x, a.y - b.y};
}

// vector < rectangle (vector in bounds)
bool operator<(const Vector2& v, const RayRectangle& b)
{
	return (v.x > b.x && b.x + b.width > v.x) && (v.y > b.y && b.y + b.height > v.y);
}

///////////////////
// Game Specific //
///////////////////

bool operator==(target const& a, target const& b)
{
	return a.getX() == b.getX() && a.getY() == b.getY() && (a.isWanted() == b.isWanted());
}

bool operator==(score_pair const& a, score_pair const& b)
{
	return a.first == b.first && a.second == b.second;
}

bool operator<(const score_pair& v, const int& b)
{
	return v.second < b;
}

bool operator>(const score_pair& v, const int& b)
{
	return v.second > b;
}

bool operator<(const score_pair a, const score_pair b)
{
	return a.second < b.second;
}

bool score_pair_sort(const score_pair a, const score_pair b)
{
	return b < a;
}

std::ostream& operator<<(std::ostream& os, std::vector<score_pair>& v)
{
	auto iter = v.begin();
	auto iEnd = v.end();
	while(iter != iEnd) os << iter->first << ": " << (iter++)->second << std::endl;

	return os;
}

#endif
