#pragma once
#include <vector>
#include <string>
#include "Math3D.h"
#include "Shader.h"
#include "Helpers.h"
#include "GLLoader.h"


class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void Draw(Shader& shader);
    void Cleanup();
    void DrawGeometryOnly();

private:
    unsigned int VBO, EBO;
    void setupMesh();
};