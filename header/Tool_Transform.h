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

    static float rotation_gizmo_radius;

    int selected_axis_index = -1;
    Vector3 start_closest_axis_ray_intersection{};
    Vector3 start_closest_ray_point{}; // used for tracing only

    Object* target{};
};
float Tool_Transform::axis_length = 4.0f;
float Tool_Transform::axis_radius = 0.1f;

float Tool_Transform::rotation_gizmo_radius = 0.7f;

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

Vector3 get_closes_axis_ray_point(
        Tool_Transform& self, 
        const Camera3D& camera, 
        const Vector3& forward_vector,
        Vector3 axis
    )
{
    auto axis_position = get_translation(self.target->transform);
    auto axis_direction = axis;

    Ray ray_ws = GetScreenToWorldRay(GetMousePosition(), camera);
    auto camera_position = ray_ws.position;
    auto camera_direction = ray_ws.direction;

    Vector3 n_hat = Vector3Normalize(Vector3CrossProduct(camera_direction, axis_direction));
    Vector3 n_hat_2 = Vector3Normalize(Vector3CrossProduct(axis_direction, n_hat));

    // Find how far away the ray is from the closest point to the axis.
    auto numerator = Vector3DotProduct(Vector3Subtract(axis_position, camera_position), n_hat_2);
    auto denominator = Vector3DotProduct(camera_direction, n_hat_2);

    // check if lines are parallel, return any point if true.
    if (abs(denominator) < 0.001)
    {
        return axis_position;
    }

    // this is how much farther the camera is from the closest point to the axis (along the camera ray).
    auto t_ray = numerator / denominator;
    auto closest_point_on_ray = camera_position + camera_direction * t_ray;

    self.start_closest_ray_point = closest_point_on_ray;

    // to get the closest point on the axis project the closest point on ray onto the axis.
    auto t_axis = Vector3DotProduct(closest_point_on_ray - axis_position, axis_direction);
    auto closest_point_on_axis = axis_position + axis_direction * t_axis;

    return closest_point_on_axis;
}

bool translation_input(Tool_Transform& self, const Camera3D& camera, const Vector3& forward_vector)
{
    Vector2 mouse_delta = GetMouseDelta();

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
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        float min_dist = 1e9;
        auto bboxes = axis_bounding_boxes();
        for (int i = 0; i < 3; ++i)
        {
            auto ray_collision = GetRayCollisionBox(ray_ms, bboxes[i]);

            if (ray_collision.hit) {
                if (ray_collision.distance < min_dist)
                {
                    min_dist = ray_collision.distance;
                    self.selected_axis_index = i;

                    self.start_closest_axis_ray_intersection = get_closes_axis_ray_point(self, 
                                                                                         camera,
                                                                                         forward_vector, 
                                                                                         axes[i]);
                }
            }
        }
    } else if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        self.selected_axis_index = -1;
        self.start_closest_axis_ray_intersection = {0, 0, 0};
        self.start_closest_ray_point = {0, 0, 0};
    }

    if (self.selected_axis_index == -1)
    {
        return false;
    }
    
    auto cur_closest_axis_ray_intersection = get_closes_axis_ray_point(self, 
                                                                        camera, 
                                                                        forward_vector, 
                                                                        axes[self.selected_axis_index]);

    auto tool_translation = cur_closest_axis_ray_intersection - self.start_closest_axis_ray_intersection;

    self.start_closest_axis_ray_intersection = cur_closest_axis_ray_intersection;

    auto cur_transform = self.target->transform;

    auto updated_object_translation = get_translation(self.target->transform) + tool_translation;
    self.target->transform = set_translation(self.target->transform, updated_object_translation);

    return true;
}

bool rotation_input(Tool_Transform& self, const Camera3D& camera, const Vector3& forward_vector)
{
    return false;
}

bool tool_transform_input(Tool_Transform& self, const Camera3D& camera, const Vector3& forward_vector)
{   
    if (translation_input(self, camera, forward_vector))
        return true;
    else if (rotation_input(self, camera, forward_vector))
        return true;

    return false;
}

void render_translation(Tool_Transform& self, const Camera3D& camera, const Vector3& forward_vector)
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
    auto translation_mat = get_translation_matrix(self.target->transform);
    for (auto bbox : bboxes)
    {
        bbox.min = Vector3Transform(bbox.min, translation_mat);
        bbox.max = Vector3Transform(bbox.max, translation_mat);
        DrawBoundingBox(bbox, RED);
    }

    if (self.selected_axis_index != -1)
    {
        DrawSphere(self.start_closest_ray_point, 0.2, RED);
    }
}

void tool_transform_render(Tool_Transform& self, const Camera3D& camera, const Vector3& forward_vector)
{
    render_translation(self, camera, forward_vector);
}