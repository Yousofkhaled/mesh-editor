#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "external/raygui.h"

#include "helper.h"
#include "tools.h"
#include "Object.h"

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

    Object* my_object = new Object();
    my_object->as_cube = Cube(3, 3, 3);
    // my_object->transform = set_translation(my_object->transform, Vector3{2, 3, 4});

    tools_init();
    app_tools.tool_transform.target = my_object;

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

                tools_render(camera, forward_vector);

                DrawGrid(40, 1.0f);
                // DrawCube({0, 0, 0}, 3, 3, 3, RED);
                DrawCubeWires(get_translation(my_object->transform), 
                                my_object->as_cube.l, 
                                my_object->as_cube.w, 
                                my_object->as_cube.h,
                                GREEN);
            EndMode3D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}