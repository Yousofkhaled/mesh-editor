#pragma once

#include "raylib.h"
#include "raymath.h"

#include "helper.h"
#include "Object.h"

#include <iostream>
#include <vector>

struct Tool_Transform
{
    static float axis_length;
    static float axis_radius;

    Object* target{};
};
float Tool_Transform::axis_length = 4.0f;
float Tool_Transform::axis_radius = 0.1f;

// assumes origin of bboxes is the origin. ray will be transformed to the model space anyway.
std::vector<BoundingBox> axis_bounding_boxes()
{
    Vector3 axes[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    std::vector<BoundingBox> ret;

    for (int i = 0; i < 3; ++i)
    {
        // treat cylindrical axes as bounding boxes for simplicity.
        BoundingBox bb;
        bb.min = {1e9, 1e9, 1e9};
        bb.max = Vector3Negate(bb.min);

        Vector3 other_axis_1 = axes[(i + 1) % 3];
        Vector3 other_axis_2 = axes[(i + 2) % 3];

        // start point
        Vector3 axis_start_point = {0, 0, 0};
        for (int j = -1; j <= 1; j += 2)
        {
            for (int k = -1; k <= 1; k += 2)
            {
                auto cur_corner = axis_start_point +
                                    other_axis_1 * Tool_Transform::axis_radius * j +
                                    other_axis_2 * Tool_Transform::axis_radius * k;

                bb.min.x = std::min(bb.min.x, cur_corner.x); bb.max.x = std::max(bb.max.x, cur_corner.x);
                bb.min.y = std::min(bb.min.y, cur_corner.y); bb.max.y = std::max(bb.max.y, cur_corner.y);
                bb.min.z = std::min(bb.min.z, cur_corner.z); bb.max.z = std::max(bb.max.z, cur_corner.z);
            }
        }

        Vector3 axis_end_point = axes[i] * Tool_Transform::axis_length;
        for (int j = -1; j <= 1; j += 2)
        {
            for (int k = -1; k <= 1; k += 2)
            {
                auto cur_corner = axis_end_point +
                                    other_axis_1 * Tool_Transform::axis_radius * j +
                                    other_axis_2 * Tool_Transform::axis_radius * k;

                bb.min.x = std::min(bb.min.x, cur_corner.x); bb.max.x = std::max(bb.max.x, cur_corner.x);
                bb.min.y = std::min(bb.min.y, cur_corner.y); bb.max.y = std::max(bb.max.y, cur_corner.y);
                bb.min.z = std::min(bb.min.z, cur_corner.z); bb.max.z = std::max(bb.max.z, cur_corner.z);
            }
        }

        ret.push_back(bb);
    }

    return ret;
}

void tool_transform_input(Tool_Transform& self, const Camera3D& camera, const Vector3& forward_vector)
{
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) == false)
        return;
    
    Vector2 mouse_delta = GetMouseDelta();
    // if (abs (mouse_delta.x) < 0.01f && abs (mouse_delta.y) < 0.01f)
    //     return;

    // eval ray
    Ray ray_ws = GetScreenToWorldRay(GetMousePosition(), camera);

    // use model's inverse transform to check the ray against the model space of the translation gizmos.
    // Use the inverse of the translation only. Tool axes are axis aligned.
    auto inverse_translation_matrix = MatrixInvert(get_translation_matrix(self.target->transform));

    Ray ray_ms;
    ray_ms.position = Vector3Transform(ray_ws.position, inverse_translation_matrix);
    ray_ms.direction = ray_ws.direction;
    // Direction does not change since we only apply the inverse of the model translation. Do not change it.

    Vector3 axes[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    int selected_axis_index = -1;
    float min_dist = 1e9;
    auto bboxes = axis_bounding_boxes();
    for (int i = 0; i < 3; ++i)
    {
        auto ray_collision = GetRayCollisionBox(ray_ms, bboxes[i]);

        if (ray_collision.hit) {
            if (ray_collision.distance < min_dist)
            {
                min_dist = ray_collision.distance;
                selected_axis_index = i;
            }
            break;
        }
    }

    if (selected_axis_index == -1)
    {
        std::cout << "DID NOT COLLIDE\n";
        return;
    }
    else
    {
        std::cout << "collided with axis : " << selected_axis_index << "\n";
    }

    auto cur_transform = self.target->transform;
    auto tool_translation = axes[selected_axis_index] * 0.01 /* sensetivity */;

    auto updated_object_translation = get_translation(self.target->transform) + tool_translation;
    self.target->transform = set_translation(self.target->transform, updated_object_translation);
}

void tool_transform_render(Tool_Transform& self, const Camera3D& camera, const Vector3& forward_vector)
{
    Vector3 axes[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    Color colors[3] = {RED, GREEN, BLUE};
    auto transform = self.target->transform;
    Vector3 object_center = get_translation(transform);
    
    for (int i = 0; i < 3; ++i)
    {
        DrawCylinderEx(object_center, 
                        object_center + axes[i] * Tool_Transform::axis_length, 
                        Tool_Transform::axis_radius, 
                        Tool_Transform::axis_radius, 
                        12, 
                        colors[i]);
    }

    // Render bounding boxes of axes
    auto bboxes = axis_bounding_boxes();
    for (auto bbox : bboxes)
    {
        auto translation_mat = get_translation_matrix(self.target->transform);
        bbox.min = Vector3Transform(bbox.min, translation_mat);
        bbox.max = Vector3Transform(bbox.max, translation_mat);
        DrawBoundingBox(bbox, RED);
    }
}