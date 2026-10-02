#pragma once

#include "raylib.h"
#include "raymath.h"

#include <iostream>
#include <tuple>
#include <vector>

Matrix set_translation(const Matrix& mat, Vector3 translation)
{
    Matrix ret = mat;
    std::tie(ret.m12, ret.m13, ret.m14) = {translation.x, translation.y, translation.z};
    return ret;
}

Vector3 get_translation(const Matrix& mat)
{
    return Vector3{mat.m12, mat.m13, mat.m14};
}

Matrix get_translation_matrix(const Matrix& mat)
{
    auto t = get_translation(mat);
    return MatrixTranslate(t.x, t.y, t.z);
}

void print_vector(Vector3 v)
{
    std::cout << v.x << ", " << v.y << ", " << v.z;
}

void DrawRing3D(Vector3 center, float innerRadius, float outerRadius, int segments, Vector3 rotationAxis, float rotationAngle, Color color)
{
    if (segments < 3) segments = 3;

    rlPushMatrix();
        rlTranslatef(center.x, center.y, center.z);
        rlRotatef(rotationAngle, rotationAxis.x, rotationAxis.y, rotationAxis.z);

        std::vector<Vector3> points;
        int count = 0;

        for (int i = 0; i <= segments; i++)
        {
            // Calculate the angle for this slice
            float angle = (float)i * (2.0f * PI / segments);
            float cosA = cosf(angle);
            float sinA = sinf(angle);

            points.push_back(Vector3{cosA * outerRadius, 0.0f, sinA * outerRadius});
            points.push_back(Vector3{cosA * innerRadius, 0.0f, sinA * innerRadius});
        }

        DrawTriangleStrip3D(points.data(), points.size(), color);
    rlPopMatrix();
}
