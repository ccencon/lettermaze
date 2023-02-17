#ifndef __A4PICTURE__
#define __A4PICTURE__

#include "lettermatrix.hpp"
#include <string>
#include <unordered_map>
#include <functional>

#pragma pack(2)//bmp含有short类型，需要两字节对齐

class A4Pictrue{
    friend class A4BMPMetaInfo;
    friend class A4BMP24HeaderInfo;

private:
    A4Pictrue() {}
    ~A4Pictrue();
    A4Pictrue(const A4Pictrue&) {}
    
    //为了适配windows还是使用C库文件IO吧
    std::unordered_map<std::string, FILE*> fdc_r;

    FILE* getReadOnlyFile(const std::string& filename);
    std::string getFileNameSuffix(const std::string& filename) const;
    void fileNameHandler(std::string& filename) const;
    void writeA424BitBMPHeader(FILE* file) const;

    using rgb_fun_t = std::function<unsigned char*(int, int)>;
    void outputPPM(const std::string& filename, rgb_fun_t rgb_f);
    void outputBMP(const std::string& filename, rgb_fun_t rgb_f);
    void output(const std::string& filename, rgb_fun_t rgb_f);
public:
    static const int LENGTH_PIXES = 2105;
    static const int WIDTH_PIXES = 1487;
    static A4Pictrue& instance();

    void output_lm(std::string filename, const std::vector<std::vector<char>>& matrix, const std::vector<POS>& route);
};

class A4BMP24MetaInfo{
public:
    short bfType = 0x4d42;//开头2字节为MB,在小端模式下元数据显示为BM
    int bfSize = (A4Pictrue::LENGTH_PIXES * 3 + 1) * A4Pictrue::WIDTH_PIXES + 54;
    short bfReserved1 = 0;
    short bfReserved2 = 0;
    int bfOffBits = 54;

    const char* getInfoString() const;
};

class A4BMP24HeaderInfo{
public:
    int biSize = 40;
    int biWidth = A4Pictrue::LENGTH_PIXES;
    int biHeight = A4Pictrue::WIDTH_PIXES;
    short biPlanes = 1;
    short biBitCount = 24;
    int biCompression = 0;
    int biSizeImages = (A4Pictrue::LENGTH_PIXES * 3 + 1) * A4Pictrue::WIDTH_PIXES;
    int biXPelsPerMeter = (double)A4Pictrue::LENGTH_PIXES / 0.297 + 0.5;
    int biYPelsPerMeter = (double)A4Pictrue::WIDTH_PIXES / 0.210 + 0.5;
    int biClrUsed = 0;
    int biClrImportant = 0;

    const char* getInfoString() const;
};

#endif
