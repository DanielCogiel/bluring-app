#pragma once

#define EXPORT extern "C" __declspec(dllexport)

EXPORT int MyFunc(int, int);