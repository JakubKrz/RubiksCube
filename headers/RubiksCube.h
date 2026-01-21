#pragma once
#include <vector>
#include <string>
#include "Mesh.h"
#include "Math3D.h"
#include "Shader.h"
#include "Loader.h"
#include "Helpers.h"
#include "CubeState.h"
#include <memory>
#include <array>
#include <deque>
#include <cstdlib>

struct Cubie {
    std::unique_ptr<Mesh> mesh;
    DualQuat transform;
    LogicalCubie logic;

    Cubie() noexcept : mesh(nullptr), transform(), logic() {}
    Cubie(const Cubie&) = delete;
    Cubie& operator=(const Cubie&) = delete;
};

class RubiksCube {
    friend class Solver;
private:
    std::array<Cubie, 27> cubies;
    std::vector<Texture> sharedTextures;
    std::deque<Move> moveQueue;

public:
    RubiksCube();
    ~RubiksCube();

    void Init(const char* texturePath, std::string modelPathPrefix);
    void Draw(Shader& shader, const Mat4& globalModel);
    void QueueRotation(Axis axis, Layer layer, bool clockwise);
    void Update(float deltaTime);
    void Scramble(int movesCount);
    bool IsSolved() const;
    std::array<LogicalCubie, 27> GetLogicalState() const;

private:
    void ApplyVisualRotation(Axis axis, Layer layer, float angleDelta);
    void UpdateLogicalGrid(Axis axis, Layer layer, bool clockwise);
    void ProcessNextMove();

    bool isAnimating = false;
    float currentAngle = 0.0f;
    float targetAngle = 90.0f;
    float defaultSpeed = 360.0f;
    float rotationSpeed = 360.0f; //270.0f; // angle/sec

    float scrambleRotationSpeed = 1000.0f;
    bool isScrambling = false;

    Axis currentAxis = Axis::X;
    Layer currentLayer = Layer::Middle;
    bool currentClockwise = false;
};