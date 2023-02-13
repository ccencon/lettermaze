#include "lettermatrix.hpp"
#include <array>
#include <cstring>
#include <random>
#include <unordered_set>
#include <functional>
#include <stack>

static const std::vector<std::array<int, 2>> DIRECT_OFFSET = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};

bool operator == (const POS& lhs, const POS& rhs)
{
	return lhs.GETX() == rhs.GETX() && lhs.GETY() == rhs.GETY();
}

template<>
class std::hash<pospair_t>
{
public:
	size_t operator()(const pospair_t& pp) const
	{
		if(pp.second.GETY() > pp.first.GETY() || (pp.second.GETY() == pp.first.GETY() && pp.second.GETX() > pp.first.GETX()))
			return ((size_t)pp.second.GETX() << 48) | ((size_t)pp.second.GETY() << 32) | ((size_t)pp.first.GETX() << 16) | (size_t)pp.first.GETY();
		return ((size_t)pp.first.GETX() << 48) | ((size_t)pp.first.GETY() << 32) | ((size_t)pp.second.GETX() << 16) | (size_t)pp.second.GETY();
	}
};

bool LetterMatrix::generate(unsigned short length, unsigned short height, std::vector<std::vector<int>>& matrix, std::vector<POS>& route) const
{
	if(length < 2 || height < 2)
		return false;

	matrix = std::vector<std::vector<int>> {height, std::vector<int>{length, 0}};
	matrix[0][length - 1] = 1;
	pospair_t pp1(POS(0, length - 1), POS(0, length - 2));
	pospair_t pp2(POS(0, length - 1), POS(1, length - 2));
	pospair_t pp3(POS(0, length - 1), POS(1, length - 1));
	std::vector<pospair_t> pp_list = {pp1, pp2, pp3};
	std::unordered_set<pospair_t> pp_visited = {pp1, pp2, pp3};

	std::random_device rd; std::mt19937 gen(rd());
	while(!pp_list.empty()){
		std::uniform_int_distribution<> dis(0, pp_list.size() - 1);
		std::size_t index = dis(gen);
		pospair_t PP = pp_list[index];
		pp_list.erase(pp_list.begin() + index);

		std::function<void(POS, POS)> func = [&](POS src, POS tar){
			matrix[tar.GETY()][tar.GETX()] = matrix[src.GETY()][src.GETX()] % 26 + 1;
			for(auto&& arry : DIRECT_OFFSET){
				int _x = arry[0] + tar.GETX(), _y = arry[1] + tar.GETY();
				pospair_t _pp(tar, POS(_x, _y));
				if(_x >= 0 && _x < length && _y >= 0 && _y < height && pp_visited.find(_pp) == pp_visited.end()){
					pp_visited.insert(_pp);
					pp_list.emplace_back(_pp);
				}
			}
		};

		if(matrix[PP.first.GETY()][PP.first.GETX()] == 0)
			func(PP.second, PP.first);
		else if(matrix[PP.second.GETY()][PP.second.GETX()] == 0)
			func(PP.first, PP.second);
	}

	findWayOut(matrix, route);
	return true;
}

void LetterMatrix::findWayOut(std::vector<std::vector<int>>& matrix, std::vector<POS>& route) const
{
	int length = matrix[0].size(), height = matrix.size();
	route.emplace_back(length - 1, 0);
	std::unordered_set<pospair_t> pp_visited;
	std::stack<std::size_t> st;
	st.push(0);

	while(!st.empty()){
		std::size_t index = st.top();
		st.pop();
		if(index >= DIRECT_OFFSET.size()){
			route.pop_back();
			continue;
		}
		st.push(index + 1);

	}

	for(auto&& arry : DIRECT_OFFSET){
		int _x = arry[0] + length - 1, _y = arry[1] + 0;
		pospair_t _pp(POS(length - 1, 1), POS(_x, _y));
		if(_x >= 0 && _x < height && _y >= 0 && _y < length && pp_visited.find(_pp) == pp_visited.end()){
			pp_visited.insert(_pp);
		}
	}
}
