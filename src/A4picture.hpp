#ifndef __A4PICTURE__
#define __A4PICTURE__

#include "lettermatrix.hpp"
#include <array>
#include <string>
#include <unordered_map>
#include <functional>

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
    std::string getFileNameSuffix(const std::string& filename) const;
    void fileNameHandler(std::string& filename);

    using rgb_fun_t = std::function<unsigned char*(int, int)>;
    void outputPPM(const std::string& filename, rgb_fun_t rgb_f);
    void outputBMP(const std::string& filename, rgb_fun_t rgb_f);
public:
    static A4Pictrue& instance();

    void output_lm(std::string filename, const std::vector<std::vector<char>>& matrix, const std::vector<POS>& route);
};

#endif
