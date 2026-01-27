#pragma once
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <deque>
#include <cstdlib>
#include "Mesh.h"
#include "Math3D.h"
#include "Shader.h"
#include "Loader.h"
#include "Helpers.h"
#include "CubeState.h"

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
    std::deque<Move> history;

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
    void UndoAll();
    void UndoLast();
    bool isUndoing = false;

private:
    void ApplyVisualRotation(Axis axis, Layer layer, float angleDelta);
    void UpdateLogicalGrid(Axis axis, Layer layer, bool clockwise);
    void ProcessNextMove();
    float EaseInOut(float t);

    bool isAnimating = false;
    float currentAngle = 0.0f;
    float targetAngle = 90.0f;
    float defaultSpeed = 360.0f; //TODO remove unused
    float rotationSpeed = 360.0f; //270.0f; // angle/sec

    float scrambleRotationSpeed = 1000.0f;
    bool isScrambling = false;

    Axis currentAxis = Axis::X;
    Layer currentLayer = Layer::Middle;
    bool currentClockwise = false;

    float animationTime = 0.0f;
    float animationDuration = 0.3f;
    float defaultAnimationDuration = 0.3f;
    float scrambleAnimationDuration = 0.05f;
    float totalAngleRotated = 0.0f;

    
};