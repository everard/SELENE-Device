// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Matrix.h"
#include <limits>

namespace selene
{

        Matrix::Matrix(float a11, float a12, float a13, float a14,
                       float a21, float a22, float a23, float a24,
                       float a31, float a32, float a33, float a34,
                       float a41, float a42, float a43, float a44)
        {
                define(a11, a12, a13, a14, a21, a22, a23, a24,
                       a31, a32, a33, a34, a41, a42, a43, a44);
        }
        Matrix::~Matrix() {}

        //--------------------------------------------------------------------------------------------------------
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

        //--------------------------------------------------------------------------------------------------------
        void Matrix::identity()
        {
                a[0][0] = 1.0f; a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] = 1.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] = 1.0f; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
        }

        //--------------------------------------------------------------------------------------------------------
        void Matrix::translation(const Vector3d& vector)
        {
                a[0][0] = 1.0f; a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] = 1.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] = 1.0f; a[2][3] = 0.0f;
                a[3][0] = vector.x;
                a[3][1] = vector.y;
                a[3][2] = vector.z;
                a[3][3] = 1.0f;
        }

        //--------------------------------------------------------------------------------------------------------
        void Matrix::scale(const Vector3d& amount)
        {
                a[0][0] = amount.x;
                a[1][1] = amount.y;
                a[2][2] = amount.z;
                a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f;
                a[3][3] = 1.0f;
        }

        //--------------------------------------------------------------------------------------------------------
        void Matrix::rotationX(float angle)
        {
                float c = std::cos(angle), s = std::sin(angle);
                a[0][0] = 1.0f; a[0][1] = 0.0f; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] =    c; a[1][2] =    s; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] =   -s; a[2][2] =    c; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
        }

        //--------------------------------------------------------------------------------------------------------
        void Matrix::rotationY(float angle)
        {
                float c = std::cos(angle), s = std::sin(angle);
                a[0][0] =    c; a[0][1] = 0.0f; a[0][2] =   -s; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] = 1.0f; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] =    s; a[2][1] = 0.0f; a[2][2] =    c; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
        }

        //--------------------------------------------------------------------------------------------------------
        void Matrix::rotationZ(float angle)
        {
                float c = std::cos(angle), s = std::sin(angle);
                a[0][0] =    c; a[0][1] =    s; a[0][2] = 0.0f; a[0][3] = 0.0f;
                a[1][0] =   -s; a[1][1] =    c; a[1][2] = 0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] = 1.0f; a[2][3] = 0.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = 0.0f; a[3][3] = 1.0f;
        }

        //--------------------------------------------------------------------------------------------------------
        void Matrix::perspective(float fieldOfView, float aspectRatio, float zNear, float zFar)
        {
                float d  = 1.0f / (zNear - zFar);
                float ys = 1.0f / std::tan(fieldOfView * SELENE_PI / 360.0f);
                float xs = ys * aspectRatio;

                a[0][0] =   xs; a[0][1] = 0.0f; a[0][2] =             0.0f; a[0][3] = 0.0f;
                a[1][0] = 0.0f; a[1][1] =   ys; a[1][2] =             0.0f; a[1][3] = 0.0f;
                a[2][0] = 0.0f; a[2][1] = 0.0f; a[2][2] =        -zFar * d; a[2][3] = 1.0f;
                a[3][0] = 0.0f; a[3][1] = 0.0f; a[3][2] = zFar * zNear * d; a[3][3] = 0.0f;
        }

        //--------------------------------------------------------------------------------------------------------
        void Matrix::lookAt(const Vector3d& eyePosition, const Vector3d& targetPosition, const Vector3d& upVector)
        {
                Vector3d zAxis = targetPosition - eyePosition;
                zAxis.normalize();

                Vector3d xAxis = upVector.cross(zAxis);
                xAxis.normalize();

                Vector3d yAxis = zAxis.cross(xAxis);

                a[0][0] = xAxis.x; a[0][1] = yAxis.x; a[0][2] = zAxis.x; a[0][3] = 0.0f;
                a[1][0] = xAxis.y; a[1][1] = yAxis.y; a[1][2] = zAxis.y; a[1][3] = 0.0f;
                a[2][0] = xAxis.z; a[2][1] = yAxis.z; a[2][2] = zAxis.z; a[2][3] = 0.0f;
                a[3][0] = -xAxis.dot(eyePosition);
                a[3][1] = -yAxis.dot(eyePosition);
                a[3][2] = -zAxis.dot(eyePosition);
                a[3][3] = 1.0f;
        }

        //--------------------------------------------------------------------------------------------------------
        bool Matrix::invert()
        {
                float c[6];
                Matrix result;

                c[0] = a[2][0] * a[3][1] - a[2][1] * a[3][0];
                c[1] = a[2][0] * a[3][2] - a[2][2] * a[3][0];
                c[2] = a[2][0] * a[3][3] - a[2][3] * a[3][0];
                c[3] = a[2][1] * a[3][2] - a[2][2] * a[3][1];
                c[4] = a[2][1] * a[3][3] - a[2][3] * a[3][1];
                c[5] = a[2][2] * a[3][3] - a[2][3] * a[3][2];

                result.a[0][0] =  a[1][1] * c[5] - a[1][2] * c[4] + a[1][3] * c[3];
                result.a[0][1] = -a[1][0] * c[5] + a[1][2] * c[2] - a[1][3] * c[1];
                result.a[0][2] =  a[1][0] * c[4] - a[1][1] * c[2] + a[1][3] * c[0];
                result.a[0][3] = -a[1][0] * c[3] + a[1][1] * c[1] - a[1][2] * c[0];

                result.a[1][0] = -a[0][1] * c[5] + a[0][2] * c[4] - a[0][3] * c[3];
                result.a[1][1] =  a[0][0] * c[5] - a[0][2] * c[2] + a[0][3] * c[1];
                result.a[1][2] = -a[0][0] * c[4] + a[0][1] * c[2] - a[0][3] * c[0];
                result.a[1][3] =  a[0][0] * c[3] - a[0][1] * c[1] + a[0][2] * c[0];

                c[0] = a[0][0] * a[1][1] - a[0][1] * a[1][0];
                c[1] = a[0][0] * a[1][2] - a[0][2] * a[1][0];
                c[2] = a[0][0] * a[1][3] - a[0][3] * a[1][0];
                c[3] = a[0][1] * a[1][2] - a[0][2] * a[1][1];
                c[4] = a[0][1] * a[1][3] - a[0][3] * a[1][1];
                c[5] = a[0][2] * a[1][3] - a[0][3] * a[1][2];

                result.a[2][0] =  a[3][1] * c[5] - a[3][2] * c[4] + a[3][3] * c[3];
                result.a[2][1] = -a[3][0] * c[5] + a[3][2] * c[2] - a[3][3] * c[1];
                result.a[2][2] =  a[3][0] * c[4] - a[3][1] * c[2] + a[3][3] * c[0];
                result.a[2][3] = -a[3][0] * c[3] + a[3][1] * c[1] - a[3][2] * c[0];

                result.a[3][0] = -a[2][1] * c[5] + a[2][2] * c[4] - a[2][3] * c[3];
                result.a[3][1] =  a[2][0] * c[5] - a[2][2] * c[2] + a[2][3] * c[1];
                result.a[3][2] = -a[2][0] * c[4] + a[2][1] * c[2] - a[2][3] * c[0];
                result.a[3][3] =  a[2][0] * c[3] - a[2][1] * c[1] + a[2][2] * c[0];

                float determinant = a[0][0] * result.a[0][0] + a[0][1] * result.a[0][1] +
                                    a[0][2] * result.a[0][2] + a[0][3] * result.a[0][3];

                const float epsilon = std::numeric_limits<float>::epsilon() * 10.0f;
                if(std::fabs(determinant) < std::fabs(determinant) * epsilon)
                        return false;

                determinant = 1.0f / determinant;
                result *= determinant;
                result.transpose();

                *this = result;
                return true;
        }

        //--------------------------------------------------------------------------------------------------------
        void Matrix::transpose()
        {
                std::swap(a[0][1], a[1][0]);
                std::swap(a[0][2], a[2][0]);
                std::swap(a[0][3], a[3][0]);
                std::swap(a[1][2], a[2][1]);
                std::swap(a[1][3], a[3][1]);
                std::swap(a[2][3], a[3][2]);
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix::operator float*()
        {
                return &a[0][0];
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix::operator const float*() const
        {
                return &a[0][0];
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix& Matrix::operator +=(float scalar)
        {
                a[0][0] += scalar; a[0][1] += scalar;
                a[0][2] += scalar; a[0][3] += scalar;
                a[1][0] += scalar; a[1][1] += scalar;
                a[1][2] += scalar; a[1][3] += scalar;
                a[2][0] += scalar; a[2][1] += scalar;
                a[2][2] += scalar; a[2][3] += scalar;
                a[3][0] += scalar; a[3][1] += scalar;
                a[3][2] += scalar; a[3][3] += scalar;

                return *this;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix& Matrix::operator +=(const Matrix& matrix)
        {
                a[0][0] += matrix.a[0][0]; a[0][1] += matrix.a[0][1];
                a[0][2] += matrix.a[0][2]; a[0][3] += matrix.a[0][3];
                a[1][0] += matrix.a[1][0]; a[1][1] += matrix.a[1][1];
                a[1][2] += matrix.a[1][2]; a[1][3] += matrix.a[1][3];
                a[2][0] += matrix.a[2][0]; a[2][1] += matrix.a[2][1];
                a[2][2] += matrix.a[2][2]; a[2][3] += matrix.a[2][3];
                a[3][0] += matrix.a[3][0]; a[3][1] += matrix.a[3][1];
                a[3][2] += matrix.a[3][2]; a[3][3] += matrix.a[3][3];

                return *this;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix& Matrix::operator -=(float scalar)
        {
                a[0][0] -= scalar; a[0][1] -= scalar;
                a[0][2] -= scalar; a[0][3] -= scalar;
                a[1][0] -= scalar; a[1][1] -= scalar;
                a[1][2] -= scalar; a[1][3] -= scalar;
                a[2][0] -= scalar; a[2][1] -= scalar;
                a[2][2] -= scalar; a[2][3] -= scalar;
                a[3][0] -= scalar; a[3][1] -= scalar;
                a[3][2] -= scalar; a[3][3] -= scalar;

                return *this;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix& Matrix::operator -=(const Matrix& matrix)
        {
                a[0][0] -= matrix.a[0][0]; a[0][1] -= matrix.a[0][1];
                a[0][2] -= matrix.a[0][2]; a[0][3] -= matrix.a[0][3];
                a[1][0] -= matrix.a[1][0]; a[1][1] -= matrix.a[1][1];
                a[1][2] -= matrix.a[1][2]; a[1][3] -= matrix.a[1][3];
                a[2][0] -= matrix.a[2][0]; a[2][1] -= matrix.a[2][1];
                a[2][2] -= matrix.a[2][2]; a[2][3] -= matrix.a[2][3];
                a[3][0] -= matrix.a[3][0]; a[3][1] -= matrix.a[3][1];
                a[3][2] -= matrix.a[3][2]; a[3][3] -= matrix.a[3][3];

                return *this;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix& Matrix::operator *=(float scalar)
        {
                a[0][0] *= scalar; a[0][1] *= scalar;
                a[0][2] *= scalar; a[0][3] *= scalar;
                a[1][0] *= scalar; a[1][1] *= scalar;
                a[1][2] *= scalar; a[1][3] *= scalar;
                a[2][0] *= scalar; a[2][1] *= scalar;
                a[2][2] *= scalar; a[2][3] *= scalar;
                a[3][0] *= scalar; a[3][1] *= scalar;
                a[3][2] *= scalar; a[3][3] *= scalar;

                return *this;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix& Matrix::operator *=(Matrix matrix)
        {
                Matrix result;
                matrix.transpose();

                for(uint32_t i = 0; i < 4; ++i)
                {
                        result.a[i][0] = a[i][0] * matrix.a[0][0] + a[i][1] * matrix.a[0][1] +
                                         a[i][2] * matrix.a[0][2] + a[i][3] * matrix.a[0][3];
                        result.a[i][1] = a[i][0] * matrix.a[1][0] + a[i][1] * matrix.a[1][1] +
                                         a[i][2] * matrix.a[1][2] + a[i][3] * matrix.a[1][3];
                        result.a[i][2] = a[i][0] * matrix.a[2][0] + a[i][1] * matrix.a[2][1] +
                                         a[i][2] * matrix.a[2][2] + a[i][3] * matrix.a[2][3];
                        result.a[i][3] = a[i][0] * matrix.a[3][0] + a[i][1] * matrix.a[3][1] +
                                         a[i][2] * matrix.a[3][2] + a[i][3] * matrix.a[3][3];
                }

                *this = result;
                return *this;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix& Matrix::operator /=(float scalar)
        {
                a[0][0] /= scalar; a[0][1] /= scalar;
                a[0][2] /= scalar; a[0][3] /= scalar;
                a[1][0] /= scalar; a[1][1] /= scalar;
                a[1][2] /= scalar; a[1][3] /= scalar;
                a[2][0] /= scalar; a[2][1] /= scalar;
                a[2][2] /= scalar; a[2][3] /= scalar;
                a[3][0] /= scalar; a[3][1] /= scalar;
                a[3][2] /= scalar; a[3][3] /= scalar;

                return *this;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix& Matrix::operator /=(const Matrix& matrix)
        {
                a[0][0] /= matrix.a[0][0]; a[0][1] /= matrix.a[0][1];
                a[0][2] /= matrix.a[0][2]; a[0][3] /= matrix.a[0][3];
                a[1][0] /= matrix.a[1][0]; a[1][1] /= matrix.a[1][1];
                a[1][2] /= matrix.a[1][2]; a[1][3] /= matrix.a[1][3];
                a[2][0] /= matrix.a[2][0]; a[2][1] /= matrix.a[2][1];
                a[2][2] /= matrix.a[2][2]; a[2][3] /= matrix.a[2][3];
                a[3][0] /= matrix.a[3][0]; a[3][1] /= matrix.a[3][1];
                a[3][2] /= matrix.a[3][2]; a[3][3] /= matrix.a[3][3];

                return *this;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator +(const Matrix& matrix)
        {
                return matrix;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator +(const Matrix& matrix, float scalar)
        {
                Matrix result = matrix;
                result += scalar;
                return result;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator +(const Matrix& matrix0, const Matrix& matrix1)
        {
                Matrix result = matrix0;
                result += matrix1;
                return result;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator -(const Matrix& matrix)
        {
                return Matrix(-matrix.a[0][0], -matrix.a[0][1], -matrix.a[0][2], -matrix.a[0][3],
                              -matrix.a[1][0], -matrix.a[1][1], -matrix.a[1][2], -matrix.a[1][3],
                              -matrix.a[2][0], -matrix.a[2][1], -matrix.a[2][2], -matrix.a[2][3],
                              -matrix.a[3][0], -matrix.a[3][1], -matrix.a[3][2], -matrix.a[3][3]);
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator -(const Matrix& matrix, float scalar)
        {
                Matrix result = matrix;
                result -= scalar;
                return result;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator -(const Matrix& matrix0, const Matrix& matrix1)
        {
                Matrix result = matrix0;
                result -= matrix1;
                return result;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator *(const Matrix& matrix, float scalar)
        {
                Matrix result = matrix;
                result *= scalar;
                return result;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator *(float scalar, const Matrix& matrix)
        {
                Matrix result = matrix;
                result *= scalar;
                return result;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator *(const Matrix& matrix0, const Matrix& matrix1)
        {
                Matrix result = matrix0;
                result *= matrix1;
                return result;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator /(const Matrix& matrix, float scalar)
        {
                Matrix result = matrix;
                result /= scalar;
                return result;
        }

        //--------------------------------------------------------------------------------------------------------
        Matrix operator /(const Matrix& matrix0, const Matrix& matrix1)
        {
                Matrix result = matrix0;
                result /= matrix1;
                return result;
        }

}
