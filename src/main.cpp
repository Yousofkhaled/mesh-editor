#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

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

    Object* object_1 = new Object();
    object_1->kind = Object::KIND_CUBE;
    object_1->as_cube = Cube(3, 3, 3);
    object_1->transform = MatrixRotate({0, 0, 1}, 0 * DEG2RAD) * MatrixTranslate(2, 2, 2);

    Mesh some_mesh = GenMeshTorus(5.0f, 2.5f, 5, 8);
    Object* object_2 = new Object();
    object_2->kind = Object::KIND_MODEL;
    object_2->as_model = LoadModelFromMesh(some_mesh);
    object_2->transform = MatrixRotate({0, 0, 1}, 0 * DEG2RAD) * MatrixTranslate(-2, -2, 2);
    
    app_data.app_objects.push_back(object_1);
    app_data.app_objects.push_back(object_2);

    tools_init();

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

                for (auto obj : app_data.app_objects)
                {
                    if (obj->kind == Object::KIND_CUBE)
                    {
                        rlPushMatrix();
                            rlMultMatrixf(MatrixToFloat(obj->transform));
                            DrawCubeWires(Vector3Zero(), 
                                            obj->as_cube.l, 
                                            obj->as_cube.w, 
                                            obj->as_cube.h,
                                            GREEN);
                        rlPopMatrix();
                    }
                    else if (obj->kind == Object::KIND_MODEL)
                    {
                        rlPushMatrix();
                            rlMultMatrixf(MatrixToFloat(obj->transform));
                            DrawModel(obj->as_model, Vector3Zero(), 1.0, PURPLE);
                            DrawModelWires(obj->as_model, Vector3Zero(), 1.0, BLUE);
                        rlPopMatrix();
                    }
                }

            EndMode3D();
        EndDrawing();
    }

    for (auto obj: app_data.app_objects)
    {
        delete obj;
    }

    CloseWindow();
    return 0;
}