#include "Loader.h"
#include "Mesh.h"
#include <iostream>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Loader::LoadOBJ(const char* path, std::vector<Vertex>& out_vertices) {
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<Vec3> temp_vertices;//TODOJK zarezerwoac od razu wiecej?
    std::vector<Vec2> temp_uvs;
    std::vector<Vec3> temp_normals;

    temp_vertices.reserve(22000);//TODOJK zaraezerwowac odpowiednia ilosc
    temp_uvs.reserve(22000);
    temp_normals.reserve(22000);

    FILE* file = nullptr;
    fopen_s(&file, path, "r");
    if (file == NULL) {
        printf("Nie mo¿na otworzyæ pliku! SprawdŸ œcie¿kê.\n");
        return false;
    }

    while (1) {
        char lineHeader[128];
        int res = fscanf_s(file, "%s", lineHeader, (unsigned int)sizeof(lineHeader));
        if (res == EOF)
            break;

        if (strcmp(lineHeader, "v") == 0) {
            Vec3 vertex;
            fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            Vec2 uv;
            fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            Vec3 normal;
            fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]);

            if (matches != 9) {
                printf("Plik nie mo¿e byc parsowany przez ten prosty loader. (Sprobuj eksportowac z 'Triangulate' w Blenderze)\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        unsigned int vIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int nIndex = normalIndices[i];

        Vertex v;
        v.Position = temp_vertices[vIndex - 1];
        v.TexCoords = temp_uvs[uvIndex - 1];
        v.Normal = temp_normals[nIndex - 1];

        out_vertices.push_back(v);
    }
    fclose(file);
    return true;
}

unsigned int Loader::LoadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "Zaladowano teksture: " << path << std::endl;
    }
    else {
        std::cout << "BLAD: Nie udalo sie wczytac tekstury: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}