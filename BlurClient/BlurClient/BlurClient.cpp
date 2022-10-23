#define _CRT_SECURE_NO_WARNINGS

#include "bmp_headers.h"
#include "BitmapManager.h"

int main()
{
    BitmapManager MyManager("land2.bmp");
  //  MyManager.printImageOnConsole();
    MyManager.runBlur(10, true);

    std::cout << std::endl << "------------------------------------" << std::endl;

}
