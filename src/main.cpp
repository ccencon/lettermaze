#include "lettermatrix.hpp"
#include "A4picture.hpp"
#include "system_.hpp"
#include <iostream>

int main()
{
	using std::cout;
	using std::endl;

	LetterMatrix lm;
	std::vector<std::vector<char>> matrix;
	std::vector<POS> route;
	lm.generate_(23, 16, matrix, route);
	try{
		A4Pictrue::instance().output_lm("lm.BMP", matrix, route);
		A4Pictrue::instance().output_lm("lm.ppm", matrix, route);
	}
	catch(std::exception& e){
		cout<<e.what()<<endl;
#ifdef WINDOWS
		system("pause");
#endif
		exit(-1);
	}

#ifdef DEBUG
	for(size_t i = 0; i < matrix.size(); i++){
		for(size_t j = 0; j < matrix[0].size(); j++){
			cout.width(2);
			cout<<int((unsigned char)matrix[i][j] - 'A')<<" ";
		}
		cout<<"\n";
	}
	cout<<"route:\n";
	for(size_t i = 0; i < route.size(); i++){
		cout<<"("<<route[i].GETX()<<","<<route[i].GETY()<<")";
	}
	cout<<endl;
#endif

#ifdef WINDOWS
	system("pause");
#endif

	return 0;
}
