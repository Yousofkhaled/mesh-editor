#pragma once

#include "raylib.h"
#include "raymath.h"

#include "algorithm"

#include "Tool_Transform.h"

// constants
constexpr float global_sensetivity = 0.01f;

struct App_Tools {
    Tool_Transform tool_transform{};
} app_tools;

void tools_init();

void tools_input(Camera3D& camera, Vector3& forward_vector);
void tools_render(Camera& camera, Vector3& forward_Vector);

void tool_pan_input(Camera3D& camera, Vector3 forward_vector);
void tool_zoom_input(Camera3D& camera, Vector3 forward_vector);
void tool_rotate_input(Camera3D& camera, Vector3& forward_vector);
void reset_scene(Camera3D& camera, Vector3& forward_vector);

void tools_init()
{
    app_tools.tool_transform = Tool_Transform();
}

void tools_input(Camera3D& camera, Vector3& forward_vector)
{
    reset_scene(camera, forward_vector);

    tool_pan_input(camera, forward_vector);
    tool_zoom_input(camera, forward_vector);
    tool_rotate_input(camera, forward_vector);

    tool_transform_input(app_tools.tool_transform, camera, forward_vector);
}

void tools_render(Camera& camera, Vector3& forward_Vector)
{
    tool_transform_render(app_tools.tool_transform, camera, forward_Vector);
}

void tool_pan_input(Camera3D& camera, Vector3 forward_vector)
{
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) &&
        IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        auto delta = GetMouseDelta();

        Vector3 up = camera.up;
        Vector3 forward = forward_vector;
        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));

        camera.position += Vector3Scale(right, -delta.x * global_sensetivity);
        camera.position += Vector3Scale(up, delta.y * global_sensetivity);
        camera.target = camera.position + forward_vector;
    }
}

void tool_zoom_input(Camera3D& camera, Vector3 forward_vector)
{
    if (!(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
    {
        auto wheel = GetMouseWheelMoveV();

        if (wheel.y != 0.0f)
        {
            camera.position -= Vector3Scale(forward_vector, wheel.y);
            camera.target = camera.position + forward_vector;
        }
    }
}

void tool_rotate_input(Camera3D& camera, Vector3& forward_vector)
{
    Vector3 up = camera.up;
    Vector3 forward = forward_vector;
    Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));

    // pitch and yaw
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
    {
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            return;

        Vector2 delta = GetMouseDelta() * global_sensetivity;

        auto point_rotate_transform = MatrixIdentity();
        auto direction_rotate_transform = MatrixIdentity();

        if (abs(delta.x) > 0.01f)
        {
            point_rotate_transform = MatrixMultiply(point_rotate_transform, MatrixRotate(up, -delta.x));
        }

        if (abs(delta.y) > 0.01f)
        {
            point_rotate_transform = MatrixMultiply(point_rotate_transform, MatrixRotate(right, -delta.y));
        }

        direction_rotate_transform = point_rotate_transform;
        direction_rotate_transform.m15 = 0;

        camera.position = Vector3Transform(camera.position, point_rotate_transform);
        forward_vector = Vector3Transform(forward_vector, direction_rotate_transform);
        camera.up = Vector3Transform(camera.up, direction_rotate_transform);

        camera.target = camera.position + forward_vector;
    }
}

void reset_scene(Camera3D& camera, Vector3& forward_vector)
{
    if (IsKeyPressed(KEY_R))
    {
        forward_vector = {0, 0, -1};
        camera.position = (Vector3){ 0.0f, 0.0f, 10.0f };   
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.target = camera.position + forward_vector;
    }
}
