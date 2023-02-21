#ifndef __LETTERMATRIX__
#define __LETTERMATRIX__

#include <vector>
#include <array>
#include <functional>

class POS{
	unsigned short x;
	unsigned short y;
public:
	POS(unsigned short _x, unsigned short _y) : x(_x), y(_y) {}
	unsigned short GETX() const { return x; }
	unsigned short GETY() const { return y; }
};

using pospair_t = std::pair<POS, POS>;
bool operator == (const POS& lhs, const POS& rhs);
bool operator != (const POS& lhs, const POS& rhs);
bool operator > (const POS& lhs, const POS& rhs);
bool operator == (const pospair_t& lhs, const pospair_t& rhs);

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
	char getNextChar(char c) const;
	using find_func_t = std::function<bool(POS, POS)>;
	void findWayOut(std::vector<std::vector<char>>& matrix, std::vector<POS>& route, find_func_t func) const;
public:
	static const std::vector<std::array<int, 2>> DIRECT_OFFSET;

	bool generate(unsigned short length, unsigned short height, std::vector<std::vector<char>>& matrix, std::vector<POS>& route) const;
	bool generate_(unsigned short length, unsigned short height, std::vector<std::vector<char>>& matrix, std::vector<POS>& route) const;
};

#endif
