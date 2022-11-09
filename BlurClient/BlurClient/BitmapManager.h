#ifndef BITMAPMANAGER_H
#define BITMAPMANAGER_H

#include "bmp_headers.h"
#include <chrono>
#include <iomanip>
#include <vector>
#include <thread>

typedef int(__cdecl* MYPROC)(DWORD, DWORD);
typedef int(__cdecl* ASM_PROC)(unsigned char*, unsigned char*, DWORD, DWORD); //(imageData, bytesPerRow, linesToProcess)

class BitmapManager {
public:
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	bool isFileLoaded;
	unsigned char* imageData;
	unsigned char* blurredImageData;
	HINSTANCE hinstLibC;
	HINSTANCE hinstLibAsm;
	MYPROC handleToCBlur;
	ASM_PROC handleToAsmBlur;

	void loadBMP(const char* filename);
public:
	BitmapManager(const char* filename);
	void printImageOnConsole();
	void printBytes(int numberOfBytes, bool choice);
	void runBlur(int threadNumber, bool choice);
	void exportImage(const char* filename);
	~BitmapManager();
};

#endif