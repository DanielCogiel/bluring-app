#pragma once

#define EXPORT extern "C" __declspec(dllexport)

//EXPORT int MyFunc(int, int);
EXPORT void BlurProc(unsigned char*, unsigned char*, DWORD);
EXPORT void TestLoading(unsigned char*, unsigned char*, DWORD, DWORD);