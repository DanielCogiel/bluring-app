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
	unsigned char* imageData;
	unsigned char* blurredImageData;
	HINSTANCE hinstLibC;
	HINSTANCE hinstLibAsm;
	MYPROC handleToCBlur;
	ASM_PROC handleToAsmBlur;
	long long lastRuntime;

	void setLastRuntime(long long duration);
public: 
	bool isFileLoaded;
	void loadBMP(const char* filename);
	BitmapManager();
	void printImageOnConsole();
	void printBytes(int numberOfBytes, bool choice);
	void runBlur(int threadNumber, bool choice);
	void TestLoading(int threadNumber);
	void exportImage(const char* filename);
	long long getLastRuntime();
	void test();
	~BitmapManager();
};

#endif