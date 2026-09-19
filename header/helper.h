#pragma once

#include "raylib.h"
#include "raymath.h"

#include <tuple>

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