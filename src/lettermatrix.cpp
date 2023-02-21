#include "lettermatrix.hpp"
#include <random>
#include <unordered_set>
#include <stack>
#include <numeric>

//8方向-上下左右+4个斜角
const std::vector<std::array<int, 2>> LetterMatrix::DIRECT_OFFSET = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
//4方向-上下左右
// const std::vector<std::array<int, 2>> LetterMatrix::DIRECT_OFFSET = {{0, -1}, {-1, 0}, {1, 0}, {0, 1}};

bool operator == (const POS& lhs, const POS& rhs)
{
	return lhs.GETX() == rhs.GETX() && lhs.GETY() == rhs.GETY();
}

bool operator != (const POS& lhs, const POS& rhs)
{
	return ! operator == (lhs, rhs);
}

bool operator > (const POS& lhs, const POS& rhs)
{
	return lhs.GETY() > rhs.GETY() || (lhs.GETY() == rhs.GETY() && lhs.GETX() > rhs.GETX());
}

bool operator == (const pospair_t& lhs, const pospair_t& rhs)
{
  return (lhs.first == rhs.first && lhs.second == rhs.second) || (lhs.first == rhs.second && lhs.second == rhs.first);
}

size_t std::hash<POS>::operator()(const POS& p) const
{
	return p.GETX() << 16 | p.GETY();
}

size_t std::hash<pospair_t>::operator()(const pospair_t& pp) const
{
	if(pp.second > pp.first)
		return std::hash<decltype(pp.second)>()(pp.second) << 32 | std::hash<decltype(pp.first)>()(pp.first);
	return std::hash<decltype(pp.first)>()(pp.first) << 32 | std::hash<decltype(pp.second)>()(pp.second);
}

char LetterMatrix::getNextChar(char c) const
{
	return ((unsigned char)c - 'A' + 1) % 26 + 'A';
}

void LetterMatrix::findWayOut(std::vector<std::vector<char>>& matrix, std::vector<POS>& route, find_func_t func) const
{
	int length = matrix[0].size(), height = matrix.size();
	route.emplace_back(length - 1, 0);
	std::unordered_set<POS> p_visited;
	p_visited.emplace(length - 1, 0);
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
		int _x = route[st.size() - 1].GETX() + DIRECT_OFFSET[index][0], _y = route[st.size() - 1].GETY() + DIRECT_OFFSET[index][1];
		if(_x < 0 || _x >= length || _y < 0 || _y >= height)
			continue;
		if(p_visited.find(POS(_x, _y)) != p_visited.end())
			continue;
		if(!func(route.back(), POS(_x, _y)))
			continue;
		route.emplace_back(_x, _y);
		if(_x == 0 && _y == height - 1)
			break;
		st.push(0);
		p_visited.emplace(_x, _y);
	}
}

bool LetterMatrix::generate(unsigned short length, unsigned short height, std::vector<std::vector<char>>& matrix, std::vector<POS>& route) const
{
	if(length < 2 || height < 2)
		return false;

	matrix = std::vector<std::vector<char>> (height, std::vector<char>(length, 0));
	matrix[0][length - 1] = 'A';
	std::vector<pospair_t> pp_list;
	for(auto&& offset : DIRECT_OFFSET){
		int _x = offset[0] + length - 1, _y = offset[1] + 0;
		if(_x >= 0 && _x < length && _y >= 0 && _y < height && matrix[_y][_x] == 0)
			pp_list.emplace_back(pospair_t(POS(length - 1, 0), POS(_x, _y)));
	}

	std::random_device rd; std::mt19937 gen(rd());
	while(!pp_list.empty()){
		std::uniform_int_distribution<> dis(0, pp_list.size() - 1);
		std::size_t index = dis(gen);
		pospair_t pp = pp_list[index];
		pp_list.erase(pp_list.begin() + index);

		std::function<void(POS, POS)> func = [&](POS src, POS tar){
			matrix[tar.GETY()][tar.GETX()] = getNextChar(matrix[src.GETY()][src.GETX()]);
			for(auto&& offset : DIRECT_OFFSET){
				int _x = offset[0] + tar.GETX(), _y = offset[1] + tar.GETY();
				if(_x >= 0 && _x < length && _y >= 0 && _y < height && matrix[_y][_x] == 0)
					pp_list.emplace_back(pospair_t(tar, POS(_x, _y)));
			}
		};

		if(matrix[pp.first.GETY()][pp.first.GETX()] == 0)
			func(pp.second, pp.first);
		else if(matrix[pp.second.GETY()][pp.second.GETX()] == 0)
			func(pp.first, pp.second);
	}

	find_func_t func = [&](POS cur, POS next){
		return matrix[next.GETY()][next.GETX()] == getNextChar(matrix[cur.GETY()][cur.GETX()]);
	};
	findWayOut(matrix, route, func);
	return true;
}

bool LetterMatrix::generate_(unsigned short length, unsigned short height, std::vector<std::vector<char>>& matrix, std::vector<POS>& route) const
{
	if(length < 2 || height < 2)
		return false;

	matrix = std::vector<std::vector<char>> (height, std::vector<char>(length, 0));
	matrix[0][length - 1] = 'A';

	std::unordered_set<pospair_t> pp_visited;
	for(int w = 0; w < (int)height; w++){
		for(int l = 0; l < (int)length; l++){
			for(auto&& offset : DIRECT_OFFSET){
				int _x = offset[0] + l, _y = offset[1] + w;
				pospair_t pp(POS(l, w), POS(_x, _y));
				if(_x >= 0 && _x < length && _y >= 0 && _y < height)
					pp_visited.emplace(pp);
			}
		}
	}
	std::vector<pospair_t> pp_list(pp_visited.begin(), pp_visited.end());

	std::vector<int> union_set(length * height);
	std::iota(union_set.begin(), union_set.end(), 0);
	std::function<int(int)> UNION_ROOT = [&](int index){
		return index == union_set[index] ? index : (union_set[index] = UNION_ROOT(union_set[index]));
	};
	std::function<void(int, int)> UNION_MERGE = [&](int i1, int i2){
		union_set[UNION_ROOT(i1)] = union_set[UNION_ROOT(i2)];
	};
	std::random_device rd; std::mt19937 gen(rd());
	while(UNION_ROOT(length - 1) != UNION_ROOT((height - 1) * length)){
		std::uniform_int_distribution<> dis(0, pp_list.size() - 1);
		std::size_t index = dis(gen);
		pospair_t pp = pp_list[index];
		pp_list.erase(pp_list.begin() + index);
		if(UNION_ROOT(pp.first.GETY() * length + pp.first.GETX()) != UNION_ROOT(pp.second.GETY() * length + pp.second.GETX())){
			UNION_MERGE(pp.first.GETY() * length + pp.first.GETX(), pp.second.GETY() * length + pp.second.GETX());
			pp_visited.erase(pospair_t(pp.first, pp.second));
		}
	}
	
	find_func_t func = [&](POS cur, POS next){
		return pp_visited.find(pospair_t(cur, next)) == pp_visited.end();
	};
	findWayOut(matrix, route, func);

	pp_list.clear();
	std::unordered_set<POS> route_set = {route[0]};
	for(std::size_t i = 1; i < route.size(); i++){
		matrix[route[i].GETY()][route[i].GETX()] = ((unsigned char)matrix[route[i - 1].GETY()][route[i - 1].GETX()] - 'A' + 1) % 26 + 'A';
		route_set.emplace(route[i]);
		for(auto&& offset : DIRECT_OFFSET){
			int _x = offset[0] + route[i].GETX(), _y = offset[1] + route[i].GETY();
			if(_x >= 0 && _x < length && _y >= 0 && _y < height && matrix[_y][_x] == 0)
				pp_list.emplace_back(POS(route[i].GETX(), route[i].GETY()), POS(_x, _y));
		}
	}

	while(!pp_list.empty()){
		std::uniform_int_distribution<> dis(0, pp_list.size() - 1);
		std::size_t index = dis(gen);
		pospair_t pp = pp_list[index];
		pp_list.erase(pp_list.begin() + index);

		std::function<void(POS, POS)> func = [&](POS src, POS tar){
			matrix[tar.GETY()][tar.GETX()] = getNextChar(matrix[src.GETY()][src.GETX()]);
			for(std::size_t i = 0; i < DIRECT_OFFSET.size(); i++){//不能指向route形成环
				int _x = DIRECT_OFFSET[i][0] + tar.GETX(), _y = DIRECT_OFFSET[i][1] + tar.GETY();
				if(_x >= 0 && _x < length && _y >= 0 && _y < height && route_set.find(POS(_x, _y)) != route_set.end()\
					 && matrix[_y][_x] == getNextChar(matrix[tar.GETY()][tar.GETX()])){
					matrix[tar.GETY()][tar.GETX()] = getNextChar(matrix[tar.GETY()][tar.GETX()]);
					i = 0;
				}
			}
			for(auto&& offset : DIRECT_OFFSET){
				int _x = offset[0] + tar.GETX(), _y = offset[1] + tar.GETY();
				if(_x >= 0 && _x < length && _y >= 0 && _y < height && matrix[_y][_x] == 0)
					pp_list.emplace_back(pospair_t(tar, POS(_x, _y)));
			}
		};

		if(matrix[pp.first.GETY()][pp.first.GETX()] == 0)
			func(pp.second, pp.first);
		else if(matrix[pp.second.GETY()][pp.second.GETX()] == 0)
			func(pp.first, pp.second);
	}

	return true;
}
