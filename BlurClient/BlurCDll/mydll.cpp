#include "pch.h" 
#include "mydll.h"

#include <iostream>

void BlurProc(unsigned char* origImage, unsigned char* blurredImage,
    DWORD bytesPerRow, DWORD linesToProcess) {

    for (int i = 0; i < linesToProcess; i++) {
        for (int j = 0; j < 6; j++)
            blurredImage[j + i * bytesPerRow] = origImage[j + i * bytesPerRow];
        for (int j = 6; j < bytesPerRow - 6; j++) {
            int calculatedValue = (origImage[j + i * bytesPerRow] +
                origImage[j - 3 + i * bytesPerRow] + origImage[j - 6 + i * bytesPerRow] 
                + origImage[j + 3 + i * bytesPerRow] + origImage[j + 6 + i * bytesPerRow]) / 5;
            blurredImage[j + i * bytesPerRow] = calculatedValue;
        }
        for (int j = bytesPerRow - 6; j < bytesPerRow; j++)
            blurredImage[j + i * bytesPerRow] = origImage[j + i * bytesPerRow];
    }

    /*for (int i = 0; i < 6; i++)
        blurredImage[i] = origImage[i];
    for (int i = 6; i < bytesPerRow - 6; i++)
    {
        int calculatedValue = (origImage[i] + origImage[i - 3] + origImage[i - 6]
            + origImage[i + 3] + origImage[i + 6]) / 5;
        blurredImage[i] = calculatedValue;
    }
    for (int i = bytesPerRow - 6; i < bytesPerRow; i++)
        blurredImage[i] = origImage[i];*/
    

}

void TestLoading(unsigned char* origImage, unsigned char* blurredImage,
    DWORD bytesPerRow, DWORD linesToProcess) {

    return;
}