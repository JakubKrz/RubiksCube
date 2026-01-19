#pragma once
#include <vector>
#include "Helpers.h"
#include "Math3D.h"

struct LogicalCubie {
    int id;
    Layer gx, gy, gz;
    Color colors[6];

    LogicalCubie() = default;
    LogicalCubie(int id, Layer gx, Layer gy, Layer gz) : id(id), gx(gx), gy(gy), gz(gz)
    {
        InitColors();
    }

    void InitColors() {
        for (int i = 0; i < 6; i++) colors[i] = Color::BLACK;

        if (gy == Layer::Positive) colors[(int)Face::UP] = Color::WHITE;
        if (gy == Layer::Negative) colors[(int)Face::DOWN] = Color::YELLOW;

        if (gx == Layer::Positive) colors[(int)Face::RIGHT] = Color::GREEN;//TODO zmeienic czerowny z zielonym
        if (gx == Layer::Negative) colors[(int)Face::LEFT] = Color::ORANGE;

        if (gz == Layer::Negative) colors[(int)Face::FRONT] = Color::RED;//TODO zmeienic czerowny z zielonym
        if (gz == Layer::Positive) colors[(int)Face::BACK] = Color::BLUE;

    }

    Color GetColorAtFace(Face dir) {
        return colors[(int)dir];
    }

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

    void RotateColors(Axis axis, bool clockwise)
    {
        Color temp[6];
        for (int i = 0; i < 6; i++) temp[i] = colors[i];

        switch (axis)
        {
        case Axis::X:
            if (clockwise) {
                colors[(int)Face::BACK] = temp[(int)Face::UP];
                colors[(int)Face::DOWN] = temp[(int)Face::BACK];
                colors[(int)Face::FRONT] = temp[(int)Face::DOWN];
                colors[(int)Face::UP] = temp[(int)Face::FRONT];
            }
            else {
                colors[(int)Face::FRONT] = temp[(int)Face::UP];
                colors[(int)Face::DOWN] = temp[(int)Face::FRONT];
                colors[(int)Face::BACK] = temp[(int)Face::DOWN];
                colors[(int)Face::UP] = temp[(int)Face::BACK];
            }
            break;

        case Axis::Y:
            if (clockwise) {
                colors[(int)Face::LEFT] = temp[(int)Face::FRONT];
                colors[(int)Face::BACK] = temp[(int)Face::LEFT];
                colors[(int)Face::RIGHT] = temp[(int)Face::BACK];
                colors[(int)Face::FRONT] = temp[(int)Face::RIGHT];
            }
            else {
                colors[(int)Face::RIGHT] = temp[(int)Face::FRONT];
                colors[(int)Face::BACK] = temp[(int)Face::RIGHT];
                colors[(int)Face::LEFT] = temp[(int)Face::BACK];
                colors[(int)Face::FRONT] = temp[(int)Face::LEFT];
            }
            break;

        case Axis::Z:
            if (clockwise) {
                colors[(int)Face::RIGHT] = temp[(int)Face::UP];
                colors[(int)Face::DOWN] = temp[(int)Face::RIGHT];
                colors[(int)Face::LEFT] = temp[(int)Face::DOWN];
                colors[(int)Face::UP] = temp[(int)Face::LEFT];
            }
            else {
                colors[(int)Face::LEFT] = temp[(int)Face::UP];
                colors[(int)Face::DOWN] = temp[(int)Face::LEFT];
                colors[(int)Face::RIGHT] = temp[(int)Face::DOWN];
                colors[(int)Face::UP] = temp[(int)Face::RIGHT];
            }
            break;
        }
    }

    void UpdateLogic(Axis axis, Layer layer, bool clockwise) {

        bool isAffected = IsAffected(axis, layer);

        if (!isAffected) return;

        RotateColors(axis, clockwise);

        Layer oldX = gx;
        Layer oldY = gy;
        Layer oldZ = gz;

        switch (axis) {
        case Axis::X:
            if (clockwise) {
                gy = -oldZ;
                gz = oldY;
            }
            else {
                gy = oldZ;
                gz = -oldY;
            }
            break;

        case Axis::Y:
            if (clockwise) {
                gz = -oldX;
                gx = oldZ;
            }
            else {
                gz = oldX;
                gx = -oldZ;
            }
            break;

        case Axis::Z:
            if (clockwise) {
                gx = oldY;
                gy = -oldX;
            }
            else {
                gx = -oldY;
                gy = oldX;
            }
            break;
        }

    }

    bool IsAffected(Axis axis, Layer layer)
    {
        if (axis == Axis::X && gx == layer) return true;
        if (axis == Axis::Y && gy == layer) return true;
        if (axis == Axis::Z && gz == layer) return true;
        return false;
    }
};