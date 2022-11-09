#define _CRT_SECURE_NO_WARNINGS

#include "bmp_headers.h"
#include "BitmapManager.h"

int main()
{
    BitmapManager MyManager("land2.bmp");
  //  MyManager.printImageOnConsole();
    std::cout << "Before proc: " << std::endl;
    std::cout << "Original image" << std::endl;
    MyManager.printBytes(1920, false);
    MyManager.runBlur(16, false);
    std::cout << "After proc: " << std::endl;
    std::cout << "Blurred image" << std::endl;
    MyManager.printBytes(1920, true);
  //  MyManager.printImageOnConsole();
}
