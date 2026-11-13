#include "raymath.h"

#include "Shapes/Sphere.h"

// RTCD - Chapter 4.3.3: Bounding Sphere from Direction of Maximum Spread

/*
    For proof on why the Eigen vector with the max Eigen value of the covariance
    matrix is the axis of the maximum spread see the end of file.
*/

// Only sets a 3*3 part of the matrix
void CovarianceMatrix(Matrix &cov, Vector3 pt[], int numPts)
{
    float oon = 1.0 / numPts;
    Vector3 center{};

    for (int i = 0; i < numPts; ++i)
        center += pt[i];
    center *= oon;

    float e00, e11, e22, e01, e02, e12;
    e00 = e11 = e22 = e01 = e02 = e12 = 0.0f;

    for (int i = 0; i < numPts; ++i)
    {
        Vector3 P = pt[i] - center;

        e00 += P.x * P.x;
        e11 += P.y * P.y;
        e22 += P.z * P.z;
        e01 += P.x * P.y;
        e02 += P.x * P.z;
        e12 += P.y * P.z;
    }

    /*
        typedef struct Matrix {
            float m0, m4, m8, m12;      // Matrix first row (4 components)
            float m1, m5, m9, m13;      // Matrix second row (4 components)
            float m2, m6, m10, m14;     // Matrix third row (4 components)
            float m3, m7, m11, m15;     // Matrix fourth row (4 components)
        } Matrix;
    */

    cov = {};

    cov.m0 = e00 * oon;
    cov.m5 = e11 * oon;
    cov.m10 = e22 * oon;
    cov.m1 = cov.m4 = e01 * oon;
    cov.m2 = cov.m8 = e02 * oon;
    cov.m7 = cov.m10 = e12 * oon;
}

/*
    Proof on why the Eigen vector with the max Eigen value of the covariance
    matrix is the direction of the max spread.

    Assume that the vertices are centered at the origin (mean = the Zero vector).
    Check the variance when data is projected onto an arbitrary unit vector V.
        Proj(X_i, V) = Dot(X_i, V) * V
        V is a unit vector, length of the projected vector is `V_transpose * X_i` [Dot Product].

    The variance rule when points are centered around U:
        sigma_squared = `(1 / N) * Sum [i = 1 -> N] (X_i - U) * (X_i - U)` [Dot Product]

        Which is the average of their squared distances from the center.

    Since points are centered around the Origin, and the length is known to be V_transpose * X_i:

        Let V_transpose be V_t:
        sigma_squared = (1 / N) * Sum [i = 1 -> N] (V_t * X_i) * (V_t * X_i) -> squared distance.

    Using matrix multiplication properties:
        sigma_squared = (1 / N) * Sum [i = 1 -> N] V_t * X_i * X_i_transpose * V

    Move V and V_t outside the sum function:
        sigma_squared = V_t * ( (1 / N) * Sum [i = 1 -> N] X_i * X_i_transpose ) * V
    
    Inner term, when vectors are multiplied to get a 3x3 matrix, is the covariance matrix:
        (1 / N) * Sum [i = 1 -> N] X_i * X_i_transpose

    ** To get the max possible sigma_squared, the unit vector V has to be in the direction of the 
    Eigen vector with the max Eigen value.
*/