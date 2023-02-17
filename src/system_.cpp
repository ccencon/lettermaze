#include "system_.hpp"
#include <cstring>

namespace SYS{

bool isLittleEndian()
{
    static const unsigned int a = 0xFFFFFF00;
    return ((unsigned char*)(&a))[0] == 0x00;
}

extern void little_ext_transform(char* tar, const char* src, long long size)
{
    if(SYS::isLittleEndian()){
        ::memcpy(tar, src, size);
    }
    else{
        for(long long i = size - 1; i >= 0; i--)
            *(tar++) = src[i];
    }
}

}
