#pragma once
#include <vector>
#include <string>
#include "Mesh.h"
#include "Math3D.h"
#include "Shader.h"
#include "Loader.h"
#include "Helpers.h"
#include <memory>
#include <array>

class RubiksCube {
private:
    struct Cubie {
        std::unique_ptr<Mesh> mesh;
        DualQuat transform;
        int id;
        Layer gx, gy, gz;

        Vec3 getGridPosition() const {
            return Vec3(
                static_cast<float>(gx),
                static_cast<float>(gy),
                static_cast<float>(gz)
            );
        }

        void setGridPosition(int x, int y, int z) {
            gx = static_cast<Layer>(x);
            gy = static_cast<Layer>(y);
            gz = static_cast<Layer>(z);
        }
    };

    std::array<Cubie, 27> cubies;
    std::vector<Texture> sharedTextures;

public:
    RubiksCube();
    ~RubiksCube();

    void Init(const char* texturePath, std::string modelPathPrefix);
    void Draw(Shader& shader, const Mat4& globalModel);
    void StartRotation(Axis axis, Layer layer, bool clockwise);
    void Update(float deltaTime);

private:
    void ApplyVisualRotation(Axis axis, Layer layer, float angleDelta);
    void UpdateLogicalGrid(Axis axis, Layer layer, bool clockwise);

    bool isAnimating = false;
    float currentAngle = 0.0f;
    float targetAngle = 90.0f;
    float rotationSpeed = 180.0f; // angle/sec

    Axis currentAxis = Axis::X;
    Layer currentLayer = Layer::Middle;
    bool currentClockwise = false;
};