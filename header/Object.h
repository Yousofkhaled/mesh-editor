#pragma once

#include "raylib.h"
#include "raymath.h"

struct Cube
{
    float l{}, w{}, h{};
    Cube() {}
    Cube (float _l, float _w, float _h) : l(_l), w(_w), h(_h) {}
};

struct Object
{
    Matrix transform = MatrixIdentity();

    enum KIND {
        KIND_CUBE,
    };
    KIND kind;

    Cube as_cube;
};