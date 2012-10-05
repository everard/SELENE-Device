// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Matrix.h"
#include <cmath>

namespace selene
{

        Matrix::Matrix(float a11, float a12, float a13, float a14,
                       float a21, float a22, float a23, float a24,
                       float a31, float a32, float a33, float a34,
                       float a41, float a42, float a43, float a44)
        {
                define(a11, a12, a13, a14,
                       a21, a22, a23, a24,
                       a31, a32, a33, a34,
                       a41, a42, a43, a44);
        }
        Matrix::~Matrix() {}

        //-------------------------------------------------------------------
        void Matrix::define(float a11, float a12, float a13, float a14,
                            float a21, float a22, float a23, float a24,
                            float a31, float a32, float a33, float a34,
                            float a41, float a42, float a43, float a44)
        {
                a[0][0] = a11; a[0][1] = a12; a[0][2] = a13; a[0][3] = a14;
                a[1][0] = a21; a[1][1] = a22; a[1][2] = a23; a[1][3] = a24;
                a[2][0] = a31; a[2][1] = a32; a[2][2] = a33; a[2][3] = a34;
                a[3][0] = a41; a[3][1] = a42; a[3][2] = a43; a[3][3] = a44;
        }

        //-------------------------------------------------------------------
        void Matrix::identity()
        {
                a[0][0] = 1.0f; a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] = 1.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] = 1.0f; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
        }

        //-------------------------------------------------------------------
        void Matrix::translation(const Vector3d& translationVector)
        {
                a[0][0] = 1.0f; a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] = 1.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] = 1.0f; a[2][3] = 0.0f;
                a[3][0] = translationVector.x;
                a[3][1] = translationVector.y;
                a[3][2] = translationVector.z;
                a[3][3] = 1.0f;
        }

        //-------------------------------------------------------------------
        void Matrix::scale(const Vector3d& scaleVector)
        {
                a[0][0] = scaleVector.x;
                a[1][1] = scaleVector.y;
                a[2][2] = scaleVector.z;
                a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f;
                a[3][3] = 1.0f;
        }

        //-------------------------------------------------------------------
        void Matrix::rotationX(float angle)
        {
                float c = cos(angle), s = sin(angle);
                a[0][0] = 1.0f; a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] =    c; a[1][2] =    s; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] =   -s; a[2][2] =    c; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
        }

        //-------------------------------------------------------------------
        void Matrix::rotationY(float angle)
        {
                float c = cos(angle), s = sin(angle);
                a[0][0] =    c; a[0][1] = 0.0f; a[0][2] =   -s; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] = 1.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] =    s; a[2][1] = 0.0f; a[2][2] =    c; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
        }

        //-------------------------------------------------------------------
        void Matrix::rotationZ(float angle)
        {
                float c = cos(angle), s = sin(angle);
                a[0][0] =    c; a[0][1] =    s; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] =   -s; a[1][1] =    c; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] = 1.0f; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
        }

        //-------------------------------------------------------------------
        void Matrix::yawPitchRoll(const Vector3d& rotationVector)
        {
                float ca = cos(rotationVector.x), cb = cos(rotationVector.y), cg = cos(rotationVector.z);
                float sa = sin(rotationVector.x), sb = sin(rotationVector.y), sg = sin(rotationVector.z);

                a[0][0] = ca * cb; a[0][1] = ca * sb * sg - sa * cg; a[0][2] = ca * sb * cg + sa * sg; a[0][3] = 0.0f;
                a[1][0] = sa * cb; a[1][1] = sa * sb * sg + ca * cg; a[1][2] = sa * sb * cg - ca * sg; a[1][3] = 0.0f;
                a[2][0] =     -sb; a[2][1] =                cb * sg; a[2][2] =                cb * cg; a[2][3] = 0.0f;
                a[3][0] =    0.0f; a[3][1] =                   0.0f; a[3][2] =                   0.0f; a[3][3] = 1.0f;
        }

        //-------------------------------------------------------------------
        void Matrix::perspective(float fov, float aspect, float zn, float zf)
        {
                float tg = tan(fov * SELENE_PI / 360.0f);
                float d  = zn - zf;
                float ys = 1.0f / tg;
                float xs = ys * aspect;

                a[0][0] =   xs; a[0][1] = 0.0f; a[0][2] =        0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] =   ys; a[1][2] =        0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] =     -zf / d; a[2][3] = 1.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = zf * zn / d; a[3][3] = 0.0f;
        }

        //-------------------------------------------------------------------
        void Matrix::lookAt(const Vector3d& eyePosition,
                            const Vector3d& targetPosition,
                            const Vector3d& upVector)
        {
                Vector3d zAxis = targetPosition - eyePosition;
                zAxis.normalize();

                Vector3d xAxis = upVector * zAxis;
                xAxis.normalize();

                Vector3d yAxis = zAxis * xAxis;

                a[0][0] = xAxis.x; a[0][1] = yAxis.x; a[0][2] = zAxis.x; a[0][3] = 0.0f;
                a[1][0] = xAxis.y; a[1][1] = yAxis.y; a[1][2] = zAxis.y; a[1][3] = 0.0f;
                a[2][0] = xAxis.z; a[2][1] = yAxis.z; a[2][2] = zAxis.z; a[2][3] = 0.0f;
                a[3][0] = -xAxis.dot(eyePosition);
                a[3][1] = -yAxis.dot(eyePosition);
                a[3][2] = -zAxis.dot(eyePosition);
                a[3][3] = 1.0f;
        }

        //-------------------------------------------------------------------
        bool Matrix::invert()
        {
                // Code contributed by Jacques Leroy <jle@star.be>.
                // Code lifted from Brian Paul's Mesa freeware OpenGL implementation.

                #define SWAP_ROWS(a, b) {float* tmp = a; (a) = (b); (b) = tmp;}
                #define MAT(matrix, r, c) matrix[r][c]

                float wtmp[4][8];
                float m0, m1, m2, m3, s;
                float* r0 = wtmp[0];
                float* r1 = wtmp[1];
                float* r2 = wtmp[2];
                float* r3 = wtmp[3];

                r0[0] = MAT(a, 0, 0);
                r0[1] = MAT(a, 0, 1);
                r0[2] = MAT(a, 0, 2);
                r0[3] = MAT(a, 0, 3);
                r0[4] = 1.0f;
                r0[5] =
                r0[6] =
                r0[7] = 0.0f;

                r1[0] = MAT(a, 1, 0);
                r1[1] = MAT(a, 1, 1);
                r1[2] = MAT(a, 1, 2);
                r1[3] = MAT(a, 1, 3);
                r1[5] = 1.0f;
                r1[4] =
                r1[6] =
                r1[7] = 0.0f,

                r2[0] = MAT(a, 2, 0);
                r2[1] = MAT(a, 2, 1);
                r2[2] = MAT(a, 2, 2);
                r2[3] = MAT(a, 2, 3);
                r2[6] = 1.0f;
                r2[4] =
                r2[5] =
                r2[7] = 0.0f;

                r3[0] = MAT(a, 3, 0);
                r3[1] = MAT(a, 3, 1);
                r3[2] = MAT(a, 3, 2);
                r3[3] = MAT(a, 3, 3);
                r3[7] = 1.0f;
                r3[4] =
                r3[5] =
                r3[6] = 0.0f;

                // choose pivot - or die
                if(fabs(r3[0]) > fabs(r2[0]))
                        SWAP_ROWS(r3, r2);

                if(fabs(r2[0]) > fabs(r1[0]))
                        SWAP_ROWS(r2, r1);

                if(fabs(r1[0]) > fabs(r0[0]))
                        SWAP_ROWS(r1, r0);

                if(r0[0] == 0.0f)
                        return false;

                // eliminate first variable
                m1 = r1[0] / r0[0];
                m2 = r2[0] / r0[0];
                m3 = r3[0] / r0[0];

                s = r0[1];
                r1[1] -= m1 * s;
                r2[1] -= m2 * s;
                r3[1] -= m3 * s;

                s = r0[2];
                r1[2] -= m1 * s;
                r2[2] -= m2 * s;
                r3[2] -= m3 * s;

                s = r0[3];
                r1[3] -= m1 * s;
                r2[3] -= m2 * s;
                r3[3] -= m3 * s;

                s = r0[4];

                if(s != 0.0f)
                {
                        r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s;
                }

                s = r0[5];

                if(s != 0.0f)
                {
                        r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s;
                }

                s = r0[6];

                if(s != 0.0f)
                {
                        r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s;
                }

                s = r0[7];

                if(s != 0.0f)
                {
                        r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s;
                }

                // choose pivot - or die
                if(fabs(r3[1]) > fabs(r2[1]))
                        SWAP_ROWS(r3, r2);

                if(fabs(r2[1]) > fabs(r1[1]))
                        SWAP_ROWS(r2, r1);

                if(r1[1] == 0.0f)
                        return false;

                // eliminate second variable
                m2     = r2[1] / r1[1]; m3     = r3[1] / r1[1];
                r2[2] -= m2 * r1[2];    r3[2] -= m3 * r1[2];
                r2[3] -= m2 * r1[3];    r3[3] -= m3 * r1[3];

                s = r1[4];

                if(0.0f != s)
                {
                        r2[4] -= m2 * s; r3[4] -= m3 * s;
                }

                s = r1[5];

                if(0.0f != s)
                {
                        r2[5] -= m2 * s; r3[5] -= m3 * s;
                }

                s = r1[6];

                if(0.0f != s)
                {
                        r2[6] -= m2 * s; r3[6] -= m3 * s;
                }

                s = r1[7];

                if(0.0f != s)
                {
                        r2[7] -= m2 * s; r3[7] -= m3 * s;
                }

                // choose pivot - or die
                if(fabs(r3[2]) > fabs(r2[2]))
                        SWAP_ROWS(r3, r2);

                if(r2[2] == 0.0f)
                        return false;

                // eliminate third variable
                m3     = r3[2] / r2[2];
                r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
                r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
                r3[7] -= m3 * r2[7];

                // last check
                if(r3[3] == 0.0f)
                        return false;

                // now back substitute row 3
                s      = 1.0f / r3[3];
                r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

                // now back substitute row 2
                m2    = r2[3];
                s     = 1.0f / r2[2];
                r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
                r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);

                m1     = r1[3];
                r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
                r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;

                m0     = r0[3];
                r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
                r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

                // now back substitute row 1
                m1    = r1[2];
                s     = 1.0f / r1[1];
                r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
                r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);

                m0     = r0[2];
                r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
                r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

                // now back substitute row 0
                m0    = r0[1];
                s     = 1.0f / r0[0];
                r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
                r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

                MAT(a, 0, 0) = r0[4]; MAT(a, 0, 1) = r0[5],
                MAT(a, 0, 2) = r0[6]; MAT(a, 0, 3) = r0[7],
                MAT(a, 1, 0) = r1[4]; MAT(a, 1, 1) = r1[5],
                MAT(a, 1, 2) = r1[6]; MAT(a, 1, 3) = r1[7],
                MAT(a, 2, 0) = r2[4]; MAT(a, 2, 1) = r2[5],
                MAT(a, 2, 2) = r2[6]; MAT(a, 2, 3) = r2[7],
                MAT(a, 3, 0) = r3[4]; MAT(a, 3, 1) = r3[5],
                MAT(a, 3, 2) = r3[6]; MAT(a, 3, 3) = r3[7];

                #undef MAT
                #undef SWAP_ROWS

                return true;
        }

        //-------------------------------------------------------------------
        void Matrix::transpose()
        {
                float tmp;

                for(register uint8_t i = 0; i < 3; ++i)
                {
                        for(register uint8_t j = i + 1; j < 4; ++j)
                        {
                                tmp = a[i][j];
                                a[i][j] = a[j][i];
                                a[j][i] = tmp;
                        }
                }
        }

        //-------------------------------------------------------------------
        Matrix::operator float*()
        {
                return &a[0][0];
        }

        //-------------------------------------------------------------------
        Matrix::operator const float*() const
        {
                return &a[0][0];
        }

        //-------------------------------------------------------------------
        Matrix& Matrix::operator =(const Matrix& matrix)
        {
                a[0][0] = matrix.a[0][0]; a[0][1] = matrix.a[0][1]; a[0][2] = matrix.a[0][2]; a[0][3] = matrix.a[0][3];
                a[1][0] = matrix.a[1][0]; a[1][1] = matrix.a[1][1]; a[1][2] = matrix.a[1][2]; a[1][3] = matrix.a[1][3];
                a[2][0] = matrix.a[2][0]; a[2][1] = matrix.a[2][1]; a[2][2] = matrix.a[2][2]; a[2][3] = matrix.a[2][3];
                a[3][0] = matrix.a[3][0]; a[3][1] = matrix.a[3][1]; a[3][2] = matrix.a[3][2]; a[3][3] = matrix.a[3][3];
                return *this;
        }

        //-------------------------------------------------------------------
        Matrix operator +(const Matrix& matrix)
        {
                return matrix;
        }

        //-------------------------------------------------------------------
        Matrix operator +(const Matrix& matrix0, const Matrix& matrix1)
        {
                return Matrix(matrix0.a[0][0] + matrix1.a[0][0], matrix0.a[0][1] + matrix1.a[0][1], matrix0.a[0][2] + matrix1.a[0][2], matrix0.a[0][3] + matrix1.a[0][3],
                              matrix0.a[1][0] + matrix1.a[1][0], matrix0.a[1][1] + matrix1.a[1][1], matrix0.a[1][2] + matrix1.a[1][2], matrix0.a[1][3] + matrix1.a[1][3],
                              matrix0.a[2][0] + matrix1.a[2][0], matrix0.a[2][1] + matrix1.a[2][1], matrix0.a[2][2] + matrix1.a[2][2], matrix0.a[2][3] + matrix1.a[2][3],
                              matrix0.a[3][0] + matrix1.a[3][0], matrix0.a[3][1] + matrix1.a[3][1], matrix0.a[3][2] + matrix1.a[3][2], matrix0.a[3][3] + matrix1.a[3][3]);
        }

        //-------------------------------------------------------------------
        Matrix operator +(const Matrix& matrix, float scalar)
        {
                return Matrix(matrix.a[0][0] + scalar, matrix.a[0][1] + scalar, matrix.a[0][2] + scalar, matrix.a[0][3] + scalar,
                              matrix.a[1][0] + scalar, matrix.a[1][1] + scalar, matrix.a[1][2] + scalar, matrix.a[1][3] + scalar,
                              matrix.a[2][0] + scalar, matrix.a[2][1] + scalar, matrix.a[2][2] + scalar, matrix.a[2][3] + scalar,
                              matrix.a[3][0] + scalar, matrix.a[3][1] + scalar, matrix.a[3][2] + scalar, matrix.a[3][3] + scalar);
        }

        //-------------------------------------------------------------------
        Matrix operator -(const Matrix& matrix0, const Matrix& matrix1)
        {
                return Matrix(matrix0.a[0][0] - matrix1.a[0][0], matrix0.a[0][1] - matrix1.a[0][1], matrix0.a[0][2] - matrix1.a[0][2], matrix0.a[0][3] - matrix1.a[0][3],
                              matrix0.a[1][0] - matrix1.a[1][0], matrix0.a[1][1] - matrix1.a[1][1], matrix0.a[1][2] - matrix1.a[1][2], matrix0.a[1][3] - matrix1.a[1][3],
                              matrix0.a[2][0] - matrix1.a[2][0], matrix0.a[2][1] - matrix1.a[2][1], matrix0.a[2][2] - matrix1.a[2][2], matrix0.a[2][3] - matrix1.a[2][3],
                              matrix0.a[3][0] - matrix1.a[3][0], matrix0.a[3][1] - matrix1.a[3][1], matrix0.a[3][2] - matrix1.a[3][2], matrix0.a[3][3] - matrix1.a[3][3]);
        }

        //-------------------------------------------------------------------
        Matrix operator -(const Matrix& matrix, float scalar)
        {
                return Matrix(matrix.a[0][0] - scalar, matrix.a[0][1] - scalar, matrix.a[0][2] - scalar, matrix.a[0][3] - scalar,
                              matrix.a[1][0] - scalar, matrix.a[1][1] - scalar, matrix.a[1][2] - scalar, matrix.a[1][3] - scalar,
                              matrix.a[2][0] - scalar, matrix.a[2][1] - scalar, matrix.a[2][2] - scalar, matrix.a[2][3] - scalar,
                              matrix.a[3][0] - scalar, matrix.a[3][1] - scalar, matrix.a[3][2] - scalar, matrix.a[3][3] - scalar);
        }

        //-------------------------------------------------------------------
        Matrix operator -(const Matrix& matrix)
        {
                return Matrix(-matrix.a[0][0], -matrix.a[0][1], -matrix.a[0][2], -matrix.a[0][3],
                              -matrix.a[1][0], -matrix.a[1][1], -matrix.a[1][2], -matrix.a[1][3],
                              -matrix.a[2][0], -matrix.a[2][1], -matrix.a[2][2], -matrix.a[2][3],
                              -matrix.a[3][0], -matrix.a[3][1], -matrix.a[3][2], -matrix.a[3][3]);
        }

        //-------------------------------------------------------------------
        Matrix operator *(const Matrix& matrix0, const Matrix& matrix1)
        {
                Matrix result;

                for(register uint8_t i = 0; i < 4; ++i)
                        for(register uint8_t j = 0; j < 4; ++j)
                                result.a[i][j] = matrix0.a[i][0] * matrix1.a[0][j] +
                                                 matrix0.a[i][1] * matrix1.a[1][j] +
                                                 matrix0.a[i][2] * matrix1.a[2][j] +
                                                 matrix0.a[i][3] * matrix1.a[3][j];

                return result;
        }

}
