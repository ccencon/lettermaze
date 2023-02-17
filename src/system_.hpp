#ifndef __SYSTEM___
#define __SYSTEM___

namespace SYS{
    extern bool isLittleEndian();
    extern void little_ext_transform(char* tar, const char* src, long long size);//小端内存转小端存储
}

#endif
