#include "RubiksCube.h"
#include <iostream>
#include <chrono>

RubiksCube::RubiksCube()
{
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
    for (int i = 0; i < cubies.size(); i++) {
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

        cubies[i].id = i;

        std::vector<unsigned int> emptyIndices;
        cubies[i].mesh = std::make_unique<Mesh>(vertices, emptyIndices, sharedTextures);

        int gx = (i % 3) - 1;
        int gy = ((i / 3) % 3) - 1;
        int gz = 1 - (i / 9);
        cubies[i].setGridPosition(gx, gy, gz);

        Vec3 startPos = cubies[i].getGridPosition() * spacing;
        Quat startRot = Quat::Identity();
        cubies[i].transform = DualQuat::FromRotationTranslation(startRot, startPos);
    }
}

void RubiksCube::Draw(Shader& shader, const Mat4& globalModel) {
    for (size_t i = 0; i < cubies.size(); i++) {
        Mat4 finalMatrix = globalModel * cubies[i].transform.ToMat4();

        shader.setMat4("model", finalMatrix);
        cubies[i].mesh->Draw(shader);
    }
}

void RubiksCube::StartRotation(Axis axis, Layer layer, bool clockwise) {
    if (isAnimating) return;

    isAnimating = true;
    currentAngle = 0.0f;
    currentAxis = axis;
    currentLayer = layer;
    currentClockwise = clockwise;
}

void RubiksCube::Update(float deltaTime) {
    if (!isAnimating) return;

    float step = rotationSpeed * deltaTime;

    if (currentAngle + step >= targetAngle) {
        step = targetAngle - currentAngle;
        float direction = currentClockwise ? -1.0f : 1.0f;
        ApplyVisualRotation(currentAxis, currentLayer, step * direction);

        UpdateLogicalGrid(currentAxis, currentLayer, currentClockwise);

        isAnimating = false;
        currentAngle = 0.0f;
    }
    else {
        float direction = currentClockwise ? -1.0f : 1.0f;
        ApplyVisualRotation(currentAxis, currentLayer, step * direction);
        currentAngle += step;
    }
}

void RubiksCube::ApplyVisualRotation(Axis axis, Layer layer, float angleDelta) {
    Vec3 rotAxis;
    switch (axis) {
    case Axis::X: rotAxis = Vec3(1, 0, 0); break;
    case Axis::Y: rotAxis = Vec3(0, 1, 0); break;
    case Axis::Z: rotAxis = Vec3(0, 0, 1); break;
    }

    Quat qRot = Quat::FromAxisAngle(rotAxis, angleDelta);

    DualQuat deltaDQ;
    deltaDQ.real = qRot;
    deltaDQ.dual = Quat(0, 0, 0, 0);

    for (auto& c : cubies) {
        bool hit = false;
        if (axis == Axis::X && c.gx == layer) hit = true;
        if (axis == Axis::Y && c.gy == layer) hit = true;
        if (axis == Axis::Z && c.gz == layer) hit = true;

        if (hit) {
            c.transform = deltaDQ * c.transform;
            c.transform.Normalize();
        }
    }
}

void RubiksCube::UpdateLogicalGrid(Axis axis, Layer layer, bool clockwise) {
    for (auto& c : cubies) {
        bool hit = false;
        if (axis == Axis::X && c.gx == layer) hit = true;
        if (axis == Axis::Y && c.gy == layer) hit = true;
        if (axis == Axis::Z && c.gz == layer) hit = true;

        if (!hit) continue;

        Layer oldX = c.gx;
        Layer oldY = c.gy;
        Layer oldZ = c.gz;

        switch (axis) {
        case Axis::X:
            if (clockwise) {
                c.gy = -oldZ;
                c.gz = oldY;
            }
            else {
                c.gy = oldZ;
                c.gz = -oldY;
            }
            break;

        case Axis::Y:
            if (clockwise) {
                c.gz = -oldX;
                c.gx = oldZ;
            }
            else {
                c.gz = oldX;
                c.gx = -oldZ;
            }
            break;

        case Axis::Z:
            if (clockwise) {
                c.gx = -oldY;
                c.gy = oldX;
            }
            else {
                c.gx = oldY;
                c.gy = -oldX;
            }
            break;
        }
    }
}