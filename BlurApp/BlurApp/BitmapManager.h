#ifndef BITMAPMANAGER_H
#define BITMAPMANAGER_H

#include "bmp_headers.h"
#include <chrono>
#include <iomanip>
#include <vector>
#include <thread>
#include <chrono>

typedef int(__cdecl* MYPROC)(unsigned char*, unsigned char*, DWORD, DWORD);
typedef int(__cdecl* ASM_PROC)(unsigned char*, unsigned char*, DWORD, DWORD); //(imageData, blurredImageData, bytesPerRow, linesToProcess)

class BitmapManager {
public:
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	//original image pointer
	unsigned char* imageData;
	//blurred image pointer 
	unsigned char* blurredImageData;
	//C++ DLL handle
	HINSTANCE hinstLibC;
	//ASM DLL handle
	HINSTANCE hinstLibAsm;
	//C++ procedure handle
	MYPROC handleToCBlur;
	//ASM procedure handle
	ASM_PROC handleToAsmBlur;
	long long lastRuntime;

	void setLastRuntime(long long duration);
public: 
	bool isFileLoaded;
	void loadBMP(const char* filename);
	BitmapManager();
	void runBlur(int threadNumber, bool choice);
	void exportImage(const char* filename);
	long long getLastRuntime();
	~BitmapManager();
};

#endif