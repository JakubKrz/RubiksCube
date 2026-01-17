#include "RubiksCube.h"
#include <iostream>
#include <chrono>
#include <future>

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
    float spacing = 0.03f;

    const size_t numThreads = std::thread::hardware_concurrency();
    std::vector<std::vector<Vertex>> loadedVertices(cubies.size());
    std::atomic<size_t> nextIndex(0);
    std::vector<std::thread> threads;

    auto loadModels = [&]() {
        while (true) {
            size_t i = nextIndex.fetch_add(1);
            if (i >= cubies.size()) break;

            std::string fullPath = modelPathPrefix + "cube" + std::to_string(i) + ".obj";
            Loader::LoadOBJ(fullPath.c_str(), loadedVertices[i]);
        }
        };

    threads.reserve(numThreads);
    for (size_t t = 0; t < numThreads; ++t) {
        threads.emplace_back(loadModels);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::vector<unsigned int> emptyIndices;
    for (int i = 0; i < cubies.size(); i++) {
        if (loadedVertices[i].empty()) continue;

        cubies[i].id = i;
        cubies[i].mesh = std::make_unique<Mesh>(
            std::move(loadedVertices[i]),
            emptyIndices,
            sharedTextures
        );

        int gx = (i % 3) - 1;
        int gy = ((i / 3) % 3) - 1;
        int gz = (i / 9) - 1;
        cubies[i].setGridPosition(gx, gy, gz);

        Vec3 startPos = cubies[i].getGridPosition() * spacing;
        startPos.z *= -1;
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

void RubiksCube::QueueRotation(Axis axis, Layer layer, bool clockwise) {
    moveQueue.push_back({ axis, layer, clockwise });
    if (!isAnimating) {
        ProcessNextMove();
    }
}

void RubiksCube::Update(float deltaTime) {
    if (!isAnimating) {
        ProcessNextMove();
        return;
    }

    float step = rotationSpeed * deltaTime;

    if (currentAngle + step >= targetAngle) {
        step = targetAngle - currentAngle;
        float direction = currentClockwise ? -1.0f : 1.0f;
        ApplyVisualRotation(currentAxis, currentLayer, step * direction);

        UpdateLogicalGrid(currentAxis, currentLayer, currentClockwise);

        isAnimating = false;
        currentAngle = 0.0f;

        ProcessNextMove();
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
                c.gx = oldY;
                c.gy = -oldX;
            }
            else {
                c.gx = -oldY;
                c.gy = oldX;
            }
            break;
        }
    }
}

void RubiksCube::ProcessNextMove() {
    if (moveQueue.empty()) return;

    MoveRequest move = moveQueue.front();
    moveQueue.pop_front();

    isAnimating = true;
    currentAngle = 0.0f;
    currentAxis = move.axis;
    currentLayer = move.layer;
    currentClockwise = move.clockwise;
}