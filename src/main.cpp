#include "lettermatrix.hpp"
#include "A4picture.hpp"
#include <iostream>

int main()
{
	LetterMatrix lm;
	std::vector<std::vector<char>> matrix;
	std::vector<POS> route;
	lm.generate(23, 16, matrix, route);
	try{
		A4Pictrue::instance().outputPPM("", matrix, route);
	}
	catch(std::exception& e){
		std::cout<<e.what()<<std::endl;
	}

#ifdef DEBUG
	using namespace std;
	for(size_t i = 0; i < matrix.size(); i++){
		for(size_t j = 0; j < matrix[0].size(); j++){
			cout.width(2);
			cout<<int((unsigned char)matrix[i][j] - 'A')<<" ";
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
