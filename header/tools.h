#pragma once

#include "raylib.h"
#include "raymath.h"

#include "algorithm"
#include <vector>

#include "Tool_Transform.h"

// constants
constexpr float global_sensetivity = 0.01f;

struct App_Tools {
    Tool_Transform tool_transform{};
} app_tools;

struct App_Data {
    std::vector<Object*> app_objects;
} app_data;

void tools_init();

void tools_input(Camera3D& camera, Vector3& forward_vector);
void tools_render(Camera& camera, Vector3& forward_Vector);

void tool_pan_input(Camera3D& camera, Vector3 forward_vector);
void tool_zoom_input(Camera3D& camera, Vector3 forward_vector);
void tool_rotate_input(Camera3D& camera, Vector3& forward_vector);
bool tool_select_input(Camera3D& camera, Vector3& forward_vector);
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

    
    if (tool_transform_input(app_tools.tool_transform, camera, forward_vector)) {}
    else if (tool_select_input(camera, forward_vector)) {}
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

bool tool_select_input(Camera3D& camera, Vector3& forward_vector)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Ray ray_ws = GetScreenToWorldRay(GetMousePosition(), camera);

        Object* target = nullptr;
        float min_dist = 1e9;

        for (auto object : app_data.app_objects)
        {
            if (object->kind == Object::KIND_CUBE)
            {
                BoundingBox bbox;
                bbox.max = {object->as_cube.l / 2.0f, object->as_cube.w / 2.0f, object->as_cube.h / 2.0f};
                bbox.min = Vector3Negate(bbox.max);

                auto inverse_matrix = MatrixInvert(object->transform);
                auto inverse_rotation_matrix = MatrixInvert(get_rotation_matrix(object->transform));

                Ray ray_ms;
                ray_ms.position = Vector3Transform(ray_ws.position, inverse_matrix);
                ray_ms.direction = Vector3Transform(ray_ws.direction, inverse_rotation_matrix);

                auto ray_collision = GetRayCollisionBox(ray_ms, bbox);

                if (ray_collision.hit && ray_collision.distance < min_dist)
                {
                    min_dist = ray_collision.distance;
                    target = object;
                }
            }
        }

        if (target)
        {
            app_tools.tool_transform.target = target;
            return true;
        }
        else
        {
            app_tools.tool_transform.target = nullptr;
            return false;
        }
    }

    return false;
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
