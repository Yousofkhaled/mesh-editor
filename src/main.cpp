#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "instructobots");

    SetTargetFPS(60);

    bool showMessageBox = false;

    Camera3D camera = { 0 };
    {
        camera.position = (Vector3){ 20.0f, 20.0f, 0.0f };
        camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };     
        camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };         
        camera.fovy = 45.0f;                               
        camera.projection = CAMERA_PERSPECTIVE;            

        SetTargetFPS(60);
    }

    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            BeginMode3D(camera);
                DrawGrid(40, 1.0f);
            EndMode3D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}