#include "A4picture.hpp"
#include <stdexcept>

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

void A4Pictrue::RGB(A4RGB& arry, const std::vector<std::vector<char>>& matrix)
{
    static const int LINE_PIXELS = 5;//线条宽度
    static const int SPACE_PIXELS = 80;//网格宽度
    static const int COMPOSE_PIXELS = LINE_PIXELS + SPACE_PIXELS;

    if(matrix.empty() || matrix[0].empty())
        throw std::runtime_error("letter matrix empty.");
    int length = matrix[0].size(), width = matrix.size();
    int frame_pixels_l = COMPOSE_PIXELS * length + LINE_PIXELS, frame_pixels_w = COMPOSE_PIXELS * width + LINE_PIXELS;
    if(frame_pixels_l > LENGTH_PIXES)
        throw std::runtime_error("letter matrix length too long.");
    if(frame_pixels_w > WIDTH_PIXES)
        throw std::runtime_error("letter matrix width too long.");

    int left_blank_pixels = (LENGTH_PIXES - frame_pixels_l) / 2;
    int right_blank_pixels = LENGTH_PIXES - frame_pixels_l - left_blank_pixels;
    int up_blank_pixels = (WIDTH_PIXES - frame_pixels_w) / 2;
    int down_blank_pixels = WIDTH_PIXES - frame_pixels_w - up_blank_pixels;
    for(int w = 1; w <= WIDTH_PIXES; w++){
        for(int l = 1; l <= LENGTH_PIXES; l++){
            int index = (w - 1) * LENGTH_PIXES + l - 1;
            if(w <= up_blank_pixels || w > WIDTH_PIXES - down_blank_pixels || l <= left_blank_pixels || l > LENGTH_PIXES - right_blank_pixels){
                arry[index][0] = 255; arry[index][1] = 255; arry[index][2] = 200;
                continue;
            }
            int _w = w - up_blank_pixels, _l = l - left_blank_pixels;
            _w -= (_w - 1) / COMPOSE_PIXELS * COMPOSE_PIXELS;
            _l -= (_l - 1) / COMPOSE_PIXELS * COMPOSE_PIXELS;
            if(_w <= LINE_PIXELS || _l <= LINE_PIXELS){
                arry[index][0] = 0; arry[index][1] = 0; arry[index][2] = 0;
                continue;
            }
            std::string filename("letterPic/" + std::string(1, matrix[w - 1][l - 1]) + ".bmp");
            FILE* file = getReadOnlyFile(filename);
            if(!file)
                throw std::runtime_error("open file(" + filename + ") failed.");
            _w = w - up_blank_pixels + LINE_PIXELS;
            _w = (_w - 1) % COMPOSE_PIXELS;
            _l = l - left_blank_pixels + LINE_PIXELS;
            _l = (_l - 1) % COMPOSE_PIXELS;
            fseek(file, 54 + ((79 - _w) * SPACE_PIXELS + _l) * 3, SEEK_SET);
            for(int k = 0; k < 3; k++){
                unsigned char c;
                if(fread(&c, 1, 1, file) <= sizeof(c))
                    throw std::runtime_error("fread file(" + filename + ") failed.");
                arry[index][0] = c; arry[index][1] = c; arry[index][2] = c == 255 ? 200 : c;
            }
        }
    }
}

void A4Pictrue::RGB(A4RGB& arry, const std::vector<POS>& route)
{

}

void A4Pictrue::outputPPM(std::string name, const std::vector<std::vector<char>>& matrix, const std::vector<POS>& route)
{
    A4RGB color;
    RGB(color, matrix);

}

void A4Pictrue::outputBMP(std::string name, const std::vector<std::vector<char>>& matrix, const std::vector<POS>& route)
{

}

void A4Pictrue::outputPPM(const std::string& filename, const A4RGB& arry)
{

}

void A4Pictrue::outputBMP(const std::string& filename, const A4RGB& arry)
{

}
