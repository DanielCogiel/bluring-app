﻿#define _CRT_SECURE_NO_WARNINGS

#include "bmp_headers.h"
#include "BitmapManager.h"

int main()
{
    BitmapManager MyManager("land.BMP");
  //  MyManager.printImageOnConsole();
    std::cout << "Before proc: " << std::endl;
    std::cout << "Original image" << std::endl;
    MyManager.printBytes(24, false);
    MyManager.runBlur(1, false);
    std::cout << "After proc: " << std::endl;
    std::cout << "Blurred image" << std::endl;
    MyManager.printBytes(24, true);
  //  MyManager.printImageOnConsole();
    MyManager.exportImage("output.bmp");
}
