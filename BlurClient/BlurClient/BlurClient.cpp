#define _CRT_SECURE_NO_WARNINGS

#include "bmp_headers.h"
#include "BitmapManager.h"

int main()
{
    BitmapManager MyManager("land2.bmp");
  //  MyManager.printImageOnConsole();
    std::cout << "Before proc: " << std::endl;
    MyManager.printBytes(30);
    MyManager.runBlur(10, false);
    std::cout << "After proc: " << std::endl;
    MyManager.printBytes(30);

    std::cout << std::endl << "------------------------------------" << std::endl;

}
