#pragma once

#include "raylib.h"
#include "raymath.h"

#include <iostream>
#include <tuple>
#include <vector>

Matrix set_translation(const Matrix& mat, Vector3 translation)
{
    Matrix ret = mat;
    std::tie(ret.m12, ret.m13, ret.m14) = {translation.x, translation.y, translation.z};
    return ret;
}

Vector3 get_translation(const Matrix& mat)
{
    return Vector3{mat.m12, mat.m13, mat.m14};
}

Matrix get_translation_matrix(const Matrix& mat)
{
    auto t = get_translation(mat);
    return MatrixTranslate(t.x, t.y, t.z);
}

Matrix get_rotation_matrix(const Matrix& mat)
{
    return set_translation(mat, Vector3{ 0, 0, 0 });
}

void print_vector(Vector3 v)
{
    std::cout << v.x << ", " << v.y << ", " << v.z;
}

RayCollision GetRayCollisionPlane(Ray ray, Vector3 point_on_plane, Vector3 plane_normal)
{
    float plane_d = Vector3DotProduct(plane_normal, point_on_plane);

    RayCollision ray_collision{};
    ray_collision.hit = false;

    ray.direction = Vector3Normalize(ray.direction);
    
    // Plane equation: ax + by + cz = -d
    // dot({a, b, c}, v0) + k * dot({a, b, c}, direction) = d

    float numerator = (plane_d - Vector3DotProduct(plane_normal, ray.position));
    float denominator = Vector3DotProduct(plane_normal, ray.direction);

    if (abs(denominator) < 0.0001) // division by zero (no solution). no hit.
    {
        return ray_collision;
    }

    float k = numerator / denominator;

    if (k < 0.0f) // direction opposite to ray. no hit.
    {
        return ray_collision;
    }
    else {
        ray_collision.hit = true;
        ray_collision.normal = plane_normal;

        ray_collision.point = Vector3Add(ray.position, ray.direction * k);
        ray_collision.distance = k;

        return ray_collision;
    }
}

RayCollision GetRayCollisionRing(
    Ray ray, 
    Vector3 ring_center,
    Vector3 plane_normal,
    float inner_radius,
    float outer_radius
)
{
    RayCollision ray_ring_collision{};

    auto ray_plane_collision = GetRayCollisionPlane(ray, ring_center, plane_normal);
    if (ray_plane_collision.hit == false) // no plane hit
        return ray_ring_collision;

    float dist = Vector3Distance(ray_plane_collision.point, ring_center);

    if (dist < inner_radius || dist > outer_radius) // no ring hit
        return ray_ring_collision;

    ray_ring_collision.hit = true;
    ray_ring_collision.distance = Vector3Distance(ray.position, ray_plane_collision.point);
    ray_ring_collision.normal = plane_normal;
    ray_ring_collision.point = ring_center + Vector3Subtract(ray_plane_collision.point, ring_center);

    return ray_ring_collision;
}

void DrawRing3D(Vector3 center, float innerRadius, float outerRadius, int segments, Vector3 rotationAxis, float rotationAngle, Color color)
{
    if (segments < 3) segments = 3;

    rlPushMatrix();
        rlTranslatef(center.x, center.y, center.z);
        rlRotatef(rotationAngle, rotationAxis.x, rotationAxis.y, rotationAxis.z);

        std::vector<Vector3> points;
        int count = 0;

        for (int i = 0; i <= segments; i++)
        {
            // Calculate the angle for this slice
            float angle = (float)i * (2.0f * PI / segments);
            float cosA = cosf(angle);
            float sinA = sinf(angle);

            points.push_back(Vector3{cosA * outerRadius, 0.0f, sinA * outerRadius});
            points.push_back(Vector3{cosA * innerRadius, 0.0f, sinA * innerRadius});
        }

        DrawTriangleStrip3D(points.data(), points.size(), color);
    rlPopMatrix();
}
