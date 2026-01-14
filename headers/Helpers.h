#pragma once
#include <windows.h>
//TODOJK uzylem stbimage,h zewnetrzena biblioteka + przEorbic pozniej metode do wczytywaniaa tekstur gdzie
#include "stb_image.h"
#include <iostream>

//TODOJK przenieœæ do soobnych klas
struct Vertex {
    Vec3 Position;
    Vec3 Normal;
    Vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

inline double GetTime() {
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER startCounter;
    static bool initialized = false;

    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&startCounter);
        initialized = true;
    }

    LARGE_INTEGER currentCounter;
    QueryPerformanceCounter(&currentCounter);

    return static_cast<double>(currentCounter.QuadPart - startCounter.QuadPart) / frequency.QuadPart;
} 