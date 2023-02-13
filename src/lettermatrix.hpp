#ifndef __LETTERMATRIX__
#define __LETTERMATRIX__

#include <vector>
#include <climits>

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

class LetterMatrix{
private:
	void findWayOut(std::vector<std::vector<int>>& matrix, std::vector<POS>& route) const;
public:
	bool generate(unsigned short length, unsigned short height, std::vector<std::vector<int>>& matrix, std::vector<POS>& route) const;
};

#endif
