#pragma once
#include "Math3D.h"
#include <algorithm>

class Camera {
public:
    float Distance;
    float Yaw;
    float Pitch;

    float Sensitivity = 0.3f;
    float ZoomSpeed = 1.0f;
    float MinDist = 2.0f;
    float MaxDist = 20.0f;

    Vec3 Position;

    Camera(float dist = 8.0f) : Distance(dist), Yaw(0.0f), Pitch(20.0f) {
        UpdatePosition();
    }

    Mat4 GetViewMatrix() {
        return Mat4::LookAt(Position, Vec3(0, 1.5, 0), Vec3(0, 1, 0));
    }

    void ProcessMouseMovement(float xoffset, float yoffset) {
        Yaw -= xoffset * Sensitivity;
        Pitch += yoffset * Sensitivity;

        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        UpdatePosition();
    }

    void ProcessMouseScroll(float yoffset) {
        Distance -= yoffset * ZoomSpeed;

        if (Distance < MinDist) Distance = MinDist;
        if (Distance > MaxDist) Distance = MaxDist;

        UpdatePosition();
    }

    void Reset() {
        Distance = 8.0f;
        Yaw = 0.0f;
        Pitch = 20.0f;
        UpdatePosition();
    }

private:
    void UpdatePosition() {
        float radYaw = Yaw * 0.01745329f; // degrees2rad
        float radPitch = Pitch * 0.01745329f; //TODO 

        float hDist = Distance * cosf(radPitch);

        Position.x = hDist * sinf(radYaw);
        Position.y = Distance * sinf(radPitch);
        Position.z = hDist * cosf(radYaw);

        if (Position.y < -1.5f) {
            Position.y = -1.5f;
        }
    }
};