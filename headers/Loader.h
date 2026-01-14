#pragma once
#include <vector>
#include <string>
#include "Math3D.h"

struct Vertex;

class Loader {
public:
    static bool LoadOBJ(const char* path, std::vector<Vertex>& out_vertices);
    static unsigned int LoadTexture(const char* path);
};