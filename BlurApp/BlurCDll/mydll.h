#pragma once
#define EXPORT extern "C" __declspec(dllexport)

EXPORT void BlurProc(unsigned char*, unsigned char*, DWORD, DWORD);