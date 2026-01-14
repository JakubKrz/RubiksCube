#pragma once
#include <vector>
#include <string>
#include "Mesh.h"
#include "Math3D.h"
#include "Shader.h"
#include "Loader.h"
#include <memory>

class RubiksCube {
private:
    struct Cubie {
        std::unique_ptr<Mesh> mesh;
        Mat4 modelMatrix;
        Vec3 gridPos;
        int id;
    };
        
    std::vector<Cubie> cubies;
    std::vector<Texture> sharedTextures;

public:
    RubiksCube();
    ~RubiksCube();

    void Init(const char* texturePath, std::string modelPathPrefix);
    void Draw(Shader& shader, const Mat4& globalModel);
};