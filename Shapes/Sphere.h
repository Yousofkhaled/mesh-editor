#pragma once

#include "raymath.h"

struct Sphere
{
    Vector3 center{};
    float radius{};

    Sphere(){}
    Sphere(Vector3 center, float radius) : center(center), radius(radius){}
};

bool ShereSphereIntersectionTest(const Sphere& a, const Sphere& b)
{
    Vector3 diff = a.center - b.center;
    float dist_squared = Vector3DotProduct(diff, diff);

    float radius_sum = a.radius + b.radius;
    return dist_squared <= radius_sum * radius_sum;
}
