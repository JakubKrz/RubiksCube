#include "Loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Loader::LoadOBJ(const char* path, std::vector<Vertex>& out_vertices) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        printf("Cant open file\n");
        return false;
    }

    size_t fileSize = file.tellg();
    file.seekg(0);

    std::vector<char> buffer(fileSize + 1);
    file.read(buffer.data(), fileSize);
    buffer[fileSize] = '\0';
    file.close();

    std::vector<Vec3> temp_vertices;
    std::vector<Vec2> temp_uvs;
    std::vector<Vec3> temp_normals;

    temp_vertices.reserve(22000);
    temp_uvs.reserve(22000);
    temp_normals.reserve(22000);
    out_vertices.reserve(66000);

    const char* ptr = buffer.data();
    const char* end = ptr + fileSize;

    while (ptr < end) {
        while (ptr < end && (*ptr == ' ' || *ptr == '\t')) ptr++;
        if (ptr >= end) break;

        if (*ptr == 'v') {
            ptr++;
            if (*ptr == ' ') {
                Vec3 v;
                ptr++;
                v.x = strtof(ptr, (char**)&ptr);
                v.y = strtof(ptr, (char**)&ptr);
                v.z = strtof(ptr, (char**)&ptr);
                temp_vertices.push_back(v);
            }
            else if (*ptr == 't') {
                Vec2 uv;
                ptr += 2;
                uv.x = strtof(ptr, (char**)&ptr);
                uv.y = strtof(ptr, (char**)&ptr);
                temp_uvs.push_back(uv);
            }
            else if (*ptr == 'n') {
                Vec3 n;
                ptr += 2;
                n.x = strtof(ptr, (char**)&ptr);
                n.y = strtof(ptr, (char**)&ptr);
                n.z = strtof(ptr, (char**)&ptr);
                temp_normals.push_back(n);
            }
        }
        else if (*ptr == 'f' && *(ptr + 1) == ' ') {
            ptr += 2;
            for (int i = 0; i < 3; i++) {
                unsigned int vIdx = strtoul(ptr, (char**)&ptr, 10);
                ptr++;
                unsigned int uvIdx = strtoul(ptr, (char**)&ptr, 10);
                ptr++;
                unsigned int nIdx = strtoul(ptr, (char**)&ptr, 10);

                Vertex v;
                v.Position = temp_vertices[vIdx - 1];
                v.TexCoords = temp_uvs[uvIdx - 1];
                v.Normal = temp_normals[nIdx - 1];
                out_vertices.push_back(v);
            }
        }
        while (ptr < end && *ptr != '\n') ptr++;
        ptr++;
    }
    return true;
}

unsigned int Loader::LoadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RGBA;
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
    }
    else {
        std::cout << "Cant load texture: " << path << std::endl;
        std::cout << "Reason: " << stbi_failure_reason() << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}