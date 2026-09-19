#pragma once

#include "raylib.h"
#include "raymath.h"

#include "helper.h"
#include "Object.h"

struct Tool_Transform
{
    Object* target{};
};

void tool_transform_input(Tool_Transform& self)
{

}

void tool_transform_render(Tool_Transform& self, const Camera3D& camera, const Vector3& forward_vector)
{
    Vector3 axes[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    Color colors[3] = {RED, GREEN, BLUE};
    auto transform = self.target->transform;
    Vector3 object_center = get_translation(transform);
    
    for (int i = 0; i < 3; ++i)
    {
        DrawCylinderEx(object_center, object_center + axes[i] * 4.0f, 0.1f, 0.1f, 6, colors[i]);
    }
}