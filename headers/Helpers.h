#pragma once
#include <windows.h>
#include <iostream>
#include <array>
#include "stb_image.h"
#include "Math3D.h"

struct DirectionalLight {
    Vec3 direction;
    Vec3 color;
};

enum class Axis { X, Y, Z };

enum class Layer {
    Negative = -1,
    Middle = 0,
    Positive = 1
};

inline Layer operator-(Layer l) {
    return static_cast<Layer>(-static_cast<int>(l));
}

inline bool operator==(Layer l, int val) {
    return static_cast<int>(l) == val;
}

enum class Color { WHITE, YELLOW, GREEN, BLUE, RED, ORANGE, BLACK };

enum class Face {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    FRONT = 4,
    BACK = 5
};

struct Move {
    Axis axis;
    Layer layer;
    bool clockwise;
};

inline Face GetFaceFromNormal(int x, int y, int z) {
    if (y > 0) return Face::UP;
    if (y < 0) return Face::DOWN;
    if (x > 0) return Face::RIGHT;
    if (x < 0) return Face::LEFT;
    if (z > 0) return Face::FRONT;
    return Face::BACK;
}

//TODOJK przenieœæ do soobnych klas?
struct Vertex {
    Vec3 Position;
    Vec3 Normal;
    Vec2 TexCoords;
};

struct Texture {
    unsigned int id = 0;
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

//DEBUG- ----------------------------------------

inline char ColorToChar(Color c) {
    switch (c) {
    case Color::WHITE:  return 'W'; // Bia³y
    case Color::YELLOW: return 'Y'; // ¯ó³ty
    case Color::GREEN:  return 'G'; // Zielony
    case Color::BLUE:   return 'B'; // Niebieski
    case Color::RED:    return 'R'; // Czerwony
    case Color::ORANGE: return 'O'; // Pomarañczowy
    case Color::BLACK: return '.';
    default: return '?';            //B³¹d
    }
}


