#include "raylib.h"
#include "raymath.h"

#include "algorithm"

// constants
constexpr float global_sensetivity = 0.01f;

void tool_pan_input(Camera3D& camera, Vector3 forward_vector);
void tool_zoom_input(Camera3D& camera, Vector3 forward_vector);
void tool_rotate_input(Camera3D& camera, Vector3& forward_vector);
void tools_input(Camera3D& camera, Vector3& forward_vector);

void tools_input(Camera3D& camera, Vector3& forward_vector)
{
    tool_pan_input(camera, forward_vector);
    tool_zoom_input(camera, forward_vector);
    tool_rotate_input(camera, forward_vector);
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
