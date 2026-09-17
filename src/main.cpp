#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

#include "algorithm"

// constants
float global_sensetivity = 0.01;

typedef Vector2 vec2;
typedef Vector3 vec3;

void tool_pan_input(Camera3D& camera, Vector3 forward_vector);
void tool_zoom_input(Camera3D& camera, Vector3 forward_vector);

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "mesh editor");

    SetTargetFPS(60);

    Camera3D camera = { 0 };
    Vector3 forward_vector = {0, 0, -1};
    {
        camera.position = (Vector3){ 0.0f, 0.0f, 10.0f };   
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
        camera.target = camera.position + forward_vector;
        camera.fovy = 45.0f;                               
        camera.projection = CAMERA_PERSPECTIVE;            

        SetTargetFPS(60);
    }

    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        tool_pan_input(camera, forward_vector);
        tool_zoom_input(camera, forward_vector);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            BeginMode3D(camera);
                DrawGrid(40, 1.0f);
                DrawCube({0, 0, 0}, 3, 3, 3, RED);
                DrawCubeWires({0, 0, 0}, 3, 3, 3, GREEN);
            EndMode3D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void tool_pan_input(Camera3D& camera, Vector3 forward_vector)
{
    if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) &&
        IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        auto delta = GetMouseDelta();

        vec3 up = camera.up;
        vec3 forward = forward_vector;
        vec3 right = Vector3Normalize(Vector3CrossProduct(forward, up));

        camera.position += Vector3Scale(right, -delta.x * global_sensetivity);
        camera.position += Vector3Scale(up, delta.y * global_sensetivity);
        camera.target = camera.position + forward_vector;
    }
}

void tool_zoom_input(Camera3D& camera, Vector3 forward_vector)
{
    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)))
    {
        auto wheel = GetMouseWheelMoveV();

        if (wheel.y != 0.0f)
        {
            camera.position -= Vector3Scale(forward_vector, wheel.y);
            camera.target = camera.position + forward_vector;
        }
    }
}