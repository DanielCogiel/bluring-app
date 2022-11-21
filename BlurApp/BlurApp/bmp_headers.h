#ifndef BMP_HEADERS_H
#define BMP_HEADERS_H

#include<windows.h>
#include <iostream>

#pragma pack(2)

struct BitmapFileHeader {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;

    void serialize() {
        std::cout << "File header" << std::endl;
        std::cout << "=========================" << std::endl;
        std::cout << this->bfType << std::endl;
        std::cout << this->bfSize << std::endl;
        std::cout << this->bfReserved1 << std::endl;
        std::cout << this->bfReserved2 << std::endl;
        std::cout << this->bfOffBits << std::endl;
        std::cout << "=========================" << std::endl;
    }
};

struct BitmapInfoHeader {
    DWORD biSize;
    DWORD biWidth;
    DWORD biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    DWORD biXPelsPerMeter;
    DWORD biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;

    void serialize() {
        std::cout << "Size: " << this->biWidth << "x" << this->biHeight << std::endl;
    }
};


#endif
