#include "lettermatrix.hpp"
#include "A4picture.hpp"
#include <iostream>

int main()
{
	LetterMatrix lm;
	std::vector<std::vector<int>> matrix;
	std::vector<POS> route;
	lm.generate(23, 16, matrix, route);

#ifdef DEBUG
	using namespace std;
	for(size_t i = 0; i < matrix.size(); i++){
		for(size_t j = 0; j < matrix[0].size(); j++){
			cout.width(2);
			cout<<matrix[i][j]<<" ";
		}
		cout<<endl;
	}
	cout<<"route:\n";
	for(size_t i = 0; i < route.size(); i++){
		cout<<"("<<route[i].GETX()<<","<<route[i].GETY()<<")";
	}
	cout<<endl;
#endif
	return 0;
}
