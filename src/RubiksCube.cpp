#include "RubiksCube.h"
#include <iostream>
#include <chrono>

RubiksCube::RubiksCube() {
    cubies.reserve(27);
}
RubiksCube::~RubiksCube() = default;

void RubiksCube::Init(const char* texturePath, std::string modelPathPrefix) {
    Texture tex;
    tex.id = Loader::LoadTexture(texturePath);
    tex.type = "diffuse";
    tex.path = texturePath;
    sharedTextures.push_back(tex);

    float spacing = 0.3f;

    auto globalStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 27; i++) {
        std::string fullPath = modelPathPrefix + "cube" + std::to_string(i) + ".obj";

        std::vector<Vertex> vertices;

        auto start = std::chrono::high_resolution_clock::now();

        if (!Loader::LoadOBJ(fullPath.c_str(), vertices)) {
            std::cout << "Blad wczytywania: " << fullPath << std::endl;
            continue;
        }
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "Cube_" << i << ": "
            << elapsed.count() << " ms | "
            << "Vertices: " << vertices.size() << std::endl;
        Cubie c;
        c.id = i;

        std::vector<unsigned int> emptyIndices;
        c.mesh = std::make_unique<Mesh>(vertices, emptyIndices, sharedTextures);

        float x = static_cast<float>(i % 3) - 1.0f;
        float y = static_cast<float>((i / 3) % 3) - 1.0f;
        float z = 1.0f - static_cast<float>(i / 9);

        c.gridPos = Vec3(x, y, z);
        c.modelMatrix = Mat4::Translation(Vec3(x * spacing, y * spacing, z * spacing));

        cubies.push_back(std::move(c));
    }
}

void RubiksCube::Draw(Shader& shader, const Mat4& globalModel) {
    for (size_t i = 0; i < cubies.size(); i++) {
        Mat4 finalMatrix = globalModel * cubies[i].modelMatrix;

        shader.setMat4("model", finalMatrix);
        cubies[i].mesh->Draw(shader);
    }
}