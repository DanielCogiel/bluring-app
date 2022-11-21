#define _CRT_SECURE_NO_WARNINGS

#include "bmp_headers.h"
#include "BitmapManager.h"

int main()
{
    BitmapManager MyManager("small.bmp");
    

   /* auto start = std::chrono::high_resolution_clock::now();
    MyManager.runBlur(1, false);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = finish - start;
    MyManager.loadBMP("small.bmp");
    start = std::chrono::high_resolution_clock::now();
    MyManager.runBlur(1, false);
    finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2 = finish -*/ //start;

   // std::cout << "Loading big size image took " << duration1.count() << " s" << std::endl;
   // std::cout << "Loading small size image took " << duration2.count() << " s" << std::endl;

 //   auto durationDiff = duration1 > duration2 ? duration1 - duration2 : duration2 - duration1;
   // std::cout << "The difference between two is " << durationDiff.count() << " s" << std::endl;
    //std::cout << "Before proc: " << std::endl;
    //std::cout << "Original image" << std::endl;
    //MyManager.printBytes(24, false);
  //  MyManager.TestLoading(1);
    MyManager.runBlur(64, true);
    //std::cout << "After proc: " << std::endl;
    //std::cout << "Blurred image" << std::endl;
    //MyManager.printBytes(24, true);

    MyManager.exportImage("outputC.bmp");

    //MyManager.TestLoading(64);
}
