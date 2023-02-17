#ifndef __LETTERMATRIX__
#define __LETTERMATRIX__

#include <vector>

class POS{
	unsigned short x;
	unsigned short y;
public:
	POS(unsigned short _x, unsigned short _y) : x(_x), y(_y) {}
	unsigned short GETX() const { return x; }
	unsigned short GETY() const { return y; }
	bool operator == (const POS& rhs) { return x == rhs.x && y == rhs.y; }
};
using pospair_t = std::pair<POS, POS>;

bool operator == (const POS& lhs, const POS& rhs);

template<>
class std::hash<POS>
{
public:
	size_t operator()(const POS& p) const;
};

template<>
class std::hash<pospair_t>
{
public:
	size_t operator()(const pospair_t& pp) const;
};

class LetterMatrix{
private:
	void findWayOut(std::vector<std::vector<char>>& matrix, std::vector<POS>& route) const;
public:
	bool generate(unsigned short length, unsigned short height, std::vector<std::vector<char>>& matrix, std::vector<POS>& route) const;
};

#endif
