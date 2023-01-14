#include "pch.h" 
#include "mydll.h"

#include <iostream>

void BlurProc(unsigned char* origImage, unsigned char* blurredImage,
    DWORD bytesPerLine, DWORD linesToProcess) {

    for (int i = 0; i < linesToProcess; i++)
        for (int j = 0; j < bytesPerLine; j++)
        {
            blurredImage[j + i * bytesPerLine] = ((origImage[j + i * bytesPerLine - 6 + 9] +
                origImage[j + i * bytesPerLine - 3 + 9] + 
                origImage[j + i * bytesPerLine - 6 + 9] + 
                origImage[j + i * bytesPerLine + 3 + 9] +
                origImage[j + i * bytesPerLine + 6 + 9]) * 3 + 3) >> 4;
        }
}