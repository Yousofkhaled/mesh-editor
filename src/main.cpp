#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

#include "tools.h"

typedef Vector2 vec2;
typedef Vector3 vec3;

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
        tools_input(camera, forward_vector);
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