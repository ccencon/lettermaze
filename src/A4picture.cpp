#include "A4picture.hpp"
#include <stdexcept>
#include <climits>
#include <algorithm>
#include <unordered_set>
#include <unistd.h>

A4Pictrue::~A4Pictrue()
{
    for(auto p : fdc_r)
        fclose(p.second);
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

void A4Pictrue::fileNameHandler(std::string& filename)
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

}

void A4Pictrue::output_lm(std::string filename, const std::vector<std::vector<char>>& matrix, const std::vector<POS>& route)
{
    fileNameHandler(filename);

    static const int LINE_PIXELS = 5;//线条宽度
    static const int SPACE_PIXELS = 80;//网格宽度
    static const int COMPOSE_PIXELS = LINE_PIXELS + SPACE_PIXELS;

    if(matrix.empty() || matrix[0].empty())
        throw std::runtime_error("letter matrix empty");
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
    rgb_fun_t func = [&](int w, int l){
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
        if(fread(color, 1, 1, file) < 1)
            throw std::runtime_error("fread file(" + filename + ") failed");
        color[1] = color[0]; color[2] = color[0] == 255 ? 200 : color[0];
        return color;
    };

    std::string suffix = getFileNameSuffix(filename);
    if(suffix == "ppm")
        outputPPM(filename, func);
    else if(suffix == "bmp")
        outputBMP(filename, func);
    else
        throw std::runtime_error("." + suffix + " is not supported");
}
