#ifndef __A4PICTURE__
#define __A4PICTURE__

#include "lettermatrix.hpp"
#include <array>
#include <string>
#include <unordered_map>

class A4Pictrue{
private:
    A4Pictrue() {}
    ~A4Pictrue();
    A4Pictrue(const A4Pictrue&) {}
    
    static const int LENGTH_PIXES = 2105;
    static const int WIDTH_PIXES = 1487;
    //为了适配windows还是使用C库文件IO吧
    std::unordered_map<std::string, FILE*> fdc_r;

    FILE* getReadOnlyFile(const std::string& filename);
    using A4RGB = std::array<std::array<unsigned char, 3> ,LENGTH_PIXES * WIDTH_PIXES>;
    void RGB(A4RGB& arry, const std::vector<std::vector<char>>& matrix);
    void RGB(A4RGB& arry, const std::vector<POS>& route);
    void outputPPM(const std::string& filename, const A4RGB& arry);
    void outputBMP(const std::string& filename, const A4RGB& arry);
public:
    static A4Pictrue& instance();

    void outputPPM(std::string name, const std::vector<std::vector<char>>& matrix, const std::vector<POS>& route);
    void outputBMP(std::string name, const std::vector<std::vector<char>>& matrix, const std::vector<POS>& route);
};

#endif
