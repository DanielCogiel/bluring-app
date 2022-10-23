#ifndef BITMAPMANAGER_H
#define BITMAPMANAGER_H

#include "bmp_headers.h"
#include <chrono>
#include <iomanip>
#include <vector>
#include <thread>

typedef int(_stdcall* ASM_PROC)(DWORD, DWORD);
typedef int(__cdecl* C_FUNC)(DWORD, DWORD);

class BitmapManager {
public:
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	bool isFileLoaded;
	unsigned char* imageData;
	HINSTANCE hinstLibC;
	HINSTANCE hinstLibAsm;
	C_FUNC handleToCBlur;
	ASM_PROC handleToAsmBlur;

	void loadBMP(const char* filename);
public:
	BitmapManager(const char* filename);
	void printImageOnConsole();
	void runBlur(int threadNumber, bool choice);
	~BitmapManager();
};

#endif