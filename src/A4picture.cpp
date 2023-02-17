#include "A4picture.hpp"
#include "system_.hpp"
#include <stdexcept>
#include <climits>
#include <algorithm>
#include <unordered_set>
#include <unistd.h>

const char* A4BMP24MetaInfo::getInfoString() const
{
    static char info[14] = "\0";
    SYS::little_ext_transform(info, (const char*)&(this->bfType), 2);
    SYS::little_ext_transform(info + 2, (const char*)&(this->bfSize), 4);
    SYS::little_ext_transform(info + 6, (const char*)&(this->bfReserved1), 2);
    SYS::little_ext_transform(info + 8, (const char*)&(this->bfReserved2), 2);
    SYS::little_ext_transform(info + 10, (const char*)&(this->bfOffBits), 4);

    return info;
}

const char* A4BMP24HeaderInfo::getInfoString() const
{
    static char info[40] = "\0";
    SYS::little_ext_transform(info, (const char*)&(this->biSize), 4);
    SYS::little_ext_transform(info + 4, (const char*)&(this->biWidth), 4);
    SYS::little_ext_transform(info + 8, (const char*)&(this->biHeight), 4);
    SYS::little_ext_transform(info + 12, (const char*)&(this->biPlanes), 2);
    SYS::little_ext_transform(info + 14, (const char*)&(this->biBitCount), 2);
    SYS::little_ext_transform(info + 16, (const char*)&(this->biCompression), 4);
    SYS::little_ext_transform(info + 20, (const char*)&(this->biSizeImages), 4);
    SYS::little_ext_transform(info + 24, (const char*)&(this->biXPelsPerMeter), 4);
    SYS::little_ext_transform(info + 28, (const char*)&(this->biYPelsPerMeter), 4);
    SYS::little_ext_transform(info + 32, (const char*)&(this->biClrUsed), 4);
    SYS::little_ext_transform(info + 36, (const char*)&(this->biClrImportant), 4);
    
    return info;
}

A4Pictrue::~A4Pictrue()
{
    for(auto p : fdc_r)
        fclose(p.second);
    fflush(nullptr);
}

A4Pictrue& A4Pictrue::instance()
{
    static A4Pictrue ins;
    return ins;
}

FILE* A4Pictrue::getReadOnlyFile(const std::string& filename)
{
    auto it = fdc_r.find(filename);
    if(it != fdc_r.end())
        return it->second;
    FILE* tmp = fopen(filename.data(), "rb");
    if(tmp)
        fdc_r[filename] = tmp;
    return tmp;
}

std::string A4Pictrue::getFileNameSuffix(const std::string& filename) const
{
    std::string suffix;
    for(int i = (int)filename.size() - 1; i >= 0 && filename[i] != '.'; i--)
        suffix.insert(0, 1, filename[i]);
    return suffix;
}

void A4Pictrue::fileNameHandler(std::string& filename) const
{
    static const std::unordered_set<char> ILLEGAL_CHAR = {'\\', '/', ':', '*', '?', '"', '<', '>', '|'};
    if(filename.empty())
        throw std::runtime_error("filename empty");
    if(filename.size() >= 128)
        throw std::runtime_error("filename too long");
    std::string suffix;
    bool isSplit = false;
    for(int i = (int)filename.size() - 1; i >= 0; i--){
        if(ILLEGAL_CHAR.find(filename[i]) != ILLEGAL_CHAR.end())
            throw std::runtime_error(std::string("filename contain illegal char:") + filename[i]);
        if(filename[i] == '.')
            isSplit = true;
        if(!isSplit)
            suffix.insert(0, 1, filename[i]);
    }
    if(suffix.empty() || suffix.size() + 1 >= filename.size())
        throw std::runtime_error("filename format error");
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), tolower);
    
    filename.resize(filename.size() - suffix.size() - 1);
    filename = "output/" + suffix + "/" + filename;
    if(access((filename + "." + suffix).data(), F_OK) == 0){
        unsigned long long index = 1;
        for(; index < ULLONG_MAX; index++){
            if(access((filename + "_" + std::to_string(index) + "." + suffix).data(), F_OK) != 0)
                break;
        }
        if(index == ULLONG_MAX)
            throw std::runtime_error("filename index out of range");
        filename += "_" + std::to_string(index);
    }
    filename += "." + suffix;
}

void A4Pictrue::writeA424BitBMPHeader(FILE* file) const
{
    static A4BMP24MetaInfo meta;
    static A4BMP24HeaderInfo header;
    static std::string info = std::string(meta.getInfoString(), 14) + std::string(header.getInfoString(), 40);
    if(fwrite(info.data(), 1, 54, file) < 54)
        throw std::runtime_error("bmp write meta header info failed");
}

void A4Pictrue::outputPPM(const std::string& filename, rgb_fun_t rgb_f)
{
    FILE* file = fopen(filename.data(), "wb");
    if(!file)
        throw std::runtime_error("open file(" + filename + ") failed");
    if(fprintf(file, "P6\n%d %d\n255\n", LENGTH_PIXES, WIDTH_PIXES) < 17)
        throw std::runtime_error("print meta info faild->" + filename);
    for(int w = 1; w <= WIDTH_PIXES; w++){
        for(int l = 1; l <= LENGTH_PIXES; l++){
            unsigned char* color = rgb_f(w, l);
            if(fwrite(color, 1, 3, file) < 3)
                throw std::runtime_error("PPM write fixels faild->" + filename);
        }
    }
    fclose(file);
}

void A4Pictrue::outputBMP(const std::string& filename, rgb_fun_t rgb_f)
{
    FILE* file = fopen(filename.data(), "wb");
    if(!file)
        throw std::runtime_error("open file(" + filename + ") failed");
    writeA424BitBMPHeader(file);
    for(int w = WIDTH_PIXES; w >= 1; w--){
        for(int l = 1; l <= LENGTH_PIXES; l++){
            unsigned char* color = rgb_f(w, l);
            static unsigned char tmp[3];
            tmp[0] = color[2]; tmp[1] = color[1]; tmp[2] = color[0];
            if(fwrite(tmp, 1, 3, file) < 3)
                throw std::runtime_error("BMP write fixels faild->" + filename);
        }
        static unsigned char c = 0;
        if(fwrite(&c, 1, 1, file) < 1)
            throw std::runtime_error("BMP byte aligned faild->" + filename);
    }
    fclose(file);
}

void A4Pictrue::output(const std::string& filename, rgb_fun_t rgb_f)
{
    std::string suffix = getFileNameSuffix(filename);
    if(suffix == "ppm")
        outputPPM(filename, rgb_f);
    else if(suffix == "bmp")
        outputBMP(filename, rgb_f);
    else
        throw std::runtime_error("." + suffix + " is not supported");
}

void A4Pictrue::output_lm(std::string filename, const std::vector<std::vector<char>>& matrix, const std::vector<POS>& route)
{
    fileNameHandler(filename);

    static const int LINE_PIXELS = 5;//线条宽度
    static const int SPACE_PIXELS = 80;//网格宽度
    static const int COMPOSE_PIXELS = LINE_PIXELS + SPACE_PIXELS;

    if(matrix.empty() || matrix[0].empty() || route.empty())
        throw std::runtime_error("letter matrix or route empty");
    int length = matrix[0].size(), width = matrix.size();
    int frame_pixels_l = COMPOSE_PIXELS * length + LINE_PIXELS, frame_pixels_w = COMPOSE_PIXELS * width + LINE_PIXELS;
    if(frame_pixels_l > LENGTH_PIXES)
        throw std::runtime_error("letter matrix length too long");
    if(frame_pixels_w > WIDTH_PIXES)
        throw std::runtime_error("letter matrix width too long");

    int left_blank_pixels = (LENGTH_PIXES - frame_pixels_l) / 2;
    int right_blank_pixels = LENGTH_PIXES - frame_pixels_l - left_blank_pixels;
    int up_blank_pixels = (WIDTH_PIXES - frame_pixels_w) / 2;
    int down_blank_pixels = WIDTH_PIXES - frame_pixels_w - up_blank_pixels;
    //字母迷宫
    rgb_fun_t func_letter = [&](int w, int l){
        static unsigned char color[3];
        //边界留空
        if(w <= up_blank_pixels || w > WIDTH_PIXES - down_blank_pixels || l <= left_blank_pixels || l > LENGTH_PIXES - right_blank_pixels){
            color[0] = 255; color[1] = 255; color[2] = 200;
            return color;
        }
        //迷宫边边
        int _w = w - up_blank_pixels, _l = l - left_blank_pixels;
        int i_w = (_w - 1) / COMPOSE_PIXELS, i_l = (_l - 1) / COMPOSE_PIXELS;
        if(_w - i_w * COMPOSE_PIXELS <= LINE_PIXELS || _l - i_l * COMPOSE_PIXELS <= LINE_PIXELS){
            color[0] = 0; color[1] = 0; color[2] = 0;
            return color;
        }
        //字母
        std::string filename("letterPic/" + std::string(1, matrix[i_w][i_l]) + ".bmp");
        FILE* file = getReadOnlyFile(filename);
        if(!file)
            throw std::runtime_error("open file(" + filename + ") failed");
        _w = (w - up_blank_pixels - LINE_PIXELS - 1) % COMPOSE_PIXELS;
        _l = (l - left_blank_pixels - LINE_PIXELS - 1) % COMPOSE_PIXELS;
        fseek(file, 54 + ((SPACE_PIXELS - 1 - _w) * SPACE_PIXELS + _l) * 3, SEEK_SET);
        if(fread(color, 1, 3, file) < 3)
            throw std::runtime_error("fread file(" + filename + ") failed");
        color[2] = color[2] == 255 ? 200 : color[2];
        if((i_w == 0 && i_l == length - 1) || (i_w == width - 1 && i_l == 0))
            color[2] = color[2] == 200 ? 0 : color[2];
        return color;
    };
    //字母迷宫-路径
    std::unordered_set<POS> pos_set;
    for(std::size_t i = 1; i < route.size() - 1; i++)
        pos_set.emplace(route[i]);
    rgb_fun_t func_route = [&](int w, int l){
        unsigned char* ret = func_letter(w, l);
        int _w = w - up_blank_pixels, _l = l - left_blank_pixels;
        int i_w = (_w - 1) / COMPOSE_PIXELS, i_l = (_l - 1) / COMPOSE_PIXELS;
        if(_w >= 0 && _l >= 0 && pos_set.find(POS(i_l, i_w)) != pos_set.end())
            ret[2] = ret[2] == 200 ? 0 : ret[2];
        return ret;
    };

    output(filename, func_letter);
    filename.insert(filename.size() - getFileNameSuffix(filename).size() - 1, "_route");
    output(filename, func_route);
}
