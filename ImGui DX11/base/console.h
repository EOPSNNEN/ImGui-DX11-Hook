#pragma once
#define LOG(fmt, ...)   printf(fmt "\n", ##__VA_ARGS__)
#include <windows.h>
#include <cstdio>

extern void createConsole();