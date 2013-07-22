// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Vector.h"
#include "Matrix.h"

namespace selene
{

        Vector2d::Vector2d(float x_, float y_): x(x_), y(y_) {}
        Vector2d::~Vector2d() {}

        //---------------------------------------------------------------------------------
        void Vector2d::define(float x_, float y_)
        {
                x = x_;
                y = y_;
        }

        //---------------------------------------------------------------------------------
        void Vector2d::define(float scalar)
        {
                x = y = scalar;
        }

        //---------------------------------------------------------------------------------
        float Vector2d::length() const
        {
                return sqrt((x * x) + (y * y));
        }

        //---------------------------------------------------------------------------------
        float Vector2d::dot(const Vector2d& vector) const
        {
                return ((x * vector.x) + (y * vector.y));
        }

        //---------------------------------------------------------------------------------
        void Vector2d::normalize()
        {
                float l = 1.0f / length();
                x *= l;
                y *= l;
        }

        //---------------------------------------------------------------------------------
        Vector2d::operator float*()
        {
                return &x;
        }

        //---------------------------------------------------------------------------------
        Vector2d::operator const float*() const
        {
                return &x;
        }

        //---------------------------------------------------------------------------------
        Vector2d& Vector2d::operator +=(float scalar)
        {
                x += scalar;
                y += scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector2d& Vector2d::operator +=(const Vector2d& vector)
        {
                x += vector.x;
                y += vector.y;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector2d& Vector2d::operator -=(float scalar)
        {
                x -= scalar;
                y -= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector2d& Vector2d::operator -=(const Vector2d& vector)
        {
                x -= vector.x;
                y -= vector.y;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector2d& Vector2d::operator *=(float scalar)
        {
                x *= scalar;
                y *= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector2d& Vector2d::operator *=(const Vector2d& vector)
        {
                x *= vector.x;
                y *= vector.y;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector2d& Vector2d::operator /=(float scalar)
        {
                x /= scalar;
                y /= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector2d& Vector2d::operator /=(const Vector2d& vector)
        {
                x /= vector.x;
                y /= vector.y;
                return *this;
        }

        //---------------------------------------------------------------------------------
        bool Vector2d::operator ==(const Vector2d& vector)
        {
                return (std::fabs(x - vector.x) <= SELENE_EPSILON &&
                        std::fabs(y - vector.y) <= SELENE_EPSILON);
        }

        Vector3d::Vector3d(float x_, float y_, float z_): x(x_), y(y_), z(z_) {}
        Vector3d::Vector3d(const Vector2d& vector, float z_):
                x(vector.x), y(vector.y), z(z_) {}
        Vector3d::~Vector3d() {}

        //---------------------------------------------------------------------------------
        void Vector3d::define(float x_, float y_, float z_)
        {
                x = x_;
                y = y_;
                z = z_;
        }

        //---------------------------------------------------------------------------------
        void Vector3d::define(const Vector2d& vector, float z_)
        {
                x = vector.x;
                y = vector.y;
                z = z_;
        }

        //---------------------------------------------------------------------------------
        void Vector3d::define(float scalar)
        {
                x = y = z = scalar;
        }

        //---------------------------------------------------------------------------------
        float Vector3d::length() const
        {
                return sqrt((x * x) + (y * y) + (z * z));
        }

        //---------------------------------------------------------------------------------
        float Vector3d::dot(const Vector3d& vector) const
        {
                return ((x * vector.x) + (y * vector.y) + (z * vector.z));
        }

        //---------------------------------------------------------------------------------
        Vector3d Vector3d::cross(const Vector3d& vector) const
        {
                return Vector3d(y * vector.z - z * vector.y,
                                z * vector.x - x * vector.z,
                                x * vector.y - y * vector.x);
        }

        //---------------------------------------------------------------------------------
        void Vector3d::normalize()
        {
                float l = 1.0f / length();
                x *= l;
                y *= l;
                z *= l;
        }

        //---------------------------------------------------------------------------------
        Vector3d Vector3d::lerp(const Vector3d& vector, float scalar) const
        {
                if(scalar < 0.0f)
                        scalar = 0.0f;
                if(scalar > 1.0f)
                        scalar = 1.0f;

                return (*this - scalar * (*this - vector));
        }

        //---------------------------------------------------------------------------------
        Vector3d Vector3d::scale(const Vector3d& vector) const
        {
                return Vector3d(x * vector.x, y * vector.y, z * vector.z);
        }

        //---------------------------------------------------------------------------------
        void Vector3d::transform(const Matrix& matrix)
        {
                *this = (*this) * matrix;
        }

        //---------------------------------------------------------------------------------
        Vector3d::operator float*()
        {
                return &x;
        }

        //---------------------------------------------------------------------------------
        Vector3d::operator const float*() const
        {
                return &x;
        }

        //---------------------------------------------------------------------------------
        Vector3d& Vector3d::operator +=(float scalar)
        {
                x += scalar;
                y += scalar;
                z += scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector3d& Vector3d::operator +=(const Vector3d& vector)
        {
                x += vector.x;
                y += vector.y;
                z += vector.z;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector3d& Vector3d::operator -=(float scalar)
        {
                x -= scalar;
                y -= scalar;
                z -= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector3d& Vector3d::operator -=(const Vector3d& vector)
        {
                x -= vector.x;
                y -= vector.y;
                z -= vector.z;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector3d& Vector3d::operator *=(float scalar)
        {
                x *= scalar;
                y *= scalar;
                z *= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector3d& Vector3d::operator *=(const Matrix& matrix)
        {
                *this = (*this) * matrix;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector3d& Vector3d::operator /=(float scalar)
        {
                x /= scalar;
                y /= scalar;
                z /= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector3d& Vector3d::operator /=(const Vector3d& vector)
        {
                x /= vector.x;
                y /= vector.y;
                z /= vector.z;
                return *this;
        }

        //---------------------------------------------------------------------------------
        bool Vector3d::operator ==(const Vector3d& vector)
        {
                return (std::fabs(x - vector.x) <= SELENE_EPSILON &&
                        std::fabs(y - vector.y) <= SELENE_EPSILON &&
                        std::fabs(z - vector.z) <= SELENE_EPSILON);
        }

        Vector4d::Vector4d(float x_, float y_, float z_, float w_):
                x(x_), y(y_), z(z_), w(w_) {}
        Vector4d::Vector4d(const Vector2d& vector, float z_, float w_):
                x(vector.x), y(vector.y), z(z_), w(w_) {}
        Vector4d::Vector4d(const Vector2d& vector0, const Vector2d& vector1):
                x(vector0.x), y(vector0.y), z(vector1.x), w(vector1.y) {}
        Vector4d::Vector4d(const Vector3d& vector, float w_):
                x(vector.x), y(vector.y), z(vector.z), w(w_) {}
        Vector4d::~Vector4d() {}

        //---------------------------------------------------------------------------------
        void Vector4d::define(float x_, float y_, float z_, float w_)
        {
                x = x_;
                y = y_;
                z = z_;
                w = w_;
        }

        //---------------------------------------------------------------------------------
        void Vector4d::define(const Vector2d& vector, float z_, float w_)
        {
                x = vector.x;
                y = vector.y;
                z = z_;
                w = w_;
        }

        //---------------------------------------------------------------------------------
        void Vector4d::define(const Vector2d& vector0, const Vector2d& vector1)
        {
                x = vector0.x;
                y = vector0.y;
                z = vector1.x;
                w = vector1.y;
        }

        //---------------------------------------------------------------------------------
        void Vector4d::define(const Vector3d& vector, float w_)
        {
                x = vector.x;
                y = vector.y;
                z = vector.z;
                w = w_;
        }

        //---------------------------------------------------------------------------------
        void Vector4d::define(float scalar)
        {
                x = y = z = w = scalar;
        }

        //---------------------------------------------------------------------------------
        Vector4d::operator float*()
        {
                return &x;
        }

        //---------------------------------------------------------------------------------
        Vector4d::operator const float*() const
        {
                return &x;
        }

        //---------------------------------------------------------------------------------
        Vector4d& Vector4d::operator +=(float scalar)
        {
                x += scalar;
                y += scalar;
                z += scalar;
                w += scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector4d& Vector4d::operator +=(const Vector4d& vector)
        {
                x += vector.x;
                y += vector.y;
                z += vector.z;
                w += vector.w;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector4d& Vector4d::operator -=(float scalar)
        {
                x -= scalar;
                y -= scalar;
                z -= scalar;
                w -= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector4d& Vector4d::operator -=(const Vector4d& vector)
        {
                x -= vector.x;
                y -= vector.y;
                z -= vector.z;
                w -= vector.w;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector4d& Vector4d::operator *=(float scalar)
        {
                x *= scalar;
                y *= scalar;
                z *= scalar;
                w *= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector4d& Vector4d::operator *=(const Vector4d& vector)
        {
                x *= vector.x;
                y *= vector.y;
                z *= vector.z;
                w *= vector.w;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector4d& Vector4d::operator /=(float scalar)
        {
                x /= scalar;
                y /= scalar;
                z /= scalar;
                w /= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector4d& Vector4d::operator /=(const Vector4d& vector)
        {
                x /= vector.x;
                y /= vector.y;
                z /= vector.z;
                w /= vector.w;
                return *this;
        }

        //---------------------------------------------------------------------------------
        bool Vector4d::operator ==(const Vector4d& vector)
        {
                return (std::fabs(x - vector.x) <= SELENE_EPSILON &&
                        std::fabs(y - vector.y) <= SELENE_EPSILON &&
                        std::fabs(z - vector.z) <= SELENE_EPSILON &&
                        std::fabs(w - vector.w) <= SELENE_EPSILON);
        }

        Quaternion::Quaternion(float x_, float y_, float z_, float w_):
                x(x_), y(y_), z(z_), w(w_) {}
        Quaternion::Quaternion(const Vector2d& vector, float z_, float w_):
                x(vector.x), y(vector.y), z(z_), w(w_) {}
        Quaternion::Quaternion(const Vector2d& vector0, const Vector2d& vector1):
                x(vector0.x), y(vector0.y), z(vector1.x), w(vector1.y) {}
        Quaternion::Quaternion(const Vector3d& vector, float w_):
                x(vector.x), y(vector.y), z(vector.z), w(w_) {}
        Quaternion::~Quaternion() {}

        //---------------------------------------------------------------------------------
        void Quaternion::define(float x_, float y_, float z_, float w_)
        {
                x = x_;
                y = y_;
                z = z_;
                w = w_;
        }

        //---------------------------------------------------------------------------------
        void Quaternion::define(const Vector2d& vector, float z_, float w_)
        {
                x = vector.x;
                y = vector.y;
                z = z_;
                w = w_;
        }

        //---------------------------------------------------------------------------------
        void Quaternion::define(const Vector2d& vector0, const Vector2d& vector1)
        {
                x = vector0.x;
                y = vector0.y;
                z = vector1.x;
                w = vector1.y;
        }

        //---------------------------------------------------------------------------------
        void Quaternion::define(const Vector3d& vector, float w_)
        {
                x = vector.x;
                y = vector.y;
                z = vector.z;
                w = w_;
        }

        //---------------------------------------------------------------------------------
        void Quaternion::define(float scalar)
        {
                x = y = z = w = scalar;
        }

        //---------------------------------------------------------------------------------
        float Quaternion::inner(const Quaternion& quaternion) const
        {
                return ((x * quaternion.x) + (y * quaternion.y) +
                        (z * quaternion.z) + (w * quaternion.w));
        }

        //---------------------------------------------------------------------------------
        float Quaternion::norm() const
        {
                return (x * x + y * y + z * z + w * w);
        }

        //---------------------------------------------------------------------------------
        void Quaternion::normalize()
        {
                float l = 1.0f / sqrt(norm());
                *this *= l;
        }

        //---------------------------------------------------------------------------------
        Quaternion Quaternion::conjugate() const
        {
                return Quaternion(-x, -y, -z, w);
        }

        //---------------------------------------------------------------------------------
        Vector3d Quaternion::rotate(const Vector3d& vector) const
        {
                Quaternion v = (*this) * Quaternion(vector, 0.0f);
                Quaternion p = v * conjugate();
                return Vector3d(p.x, p.y, p.z);
        }

        //---------------------------------------------------------------------------------
        Quaternion Quaternion::lerp(const Quaternion& quaternion, float scalar) const
        {
                if(scalar < 0.0f)
                        scalar = 0.0f;
                if(scalar > 1.0f)
                        scalar = 1.0f;

                Quaternion q = quaternion;
                float c = inner(q);
                if(c < 0.0f)
                        q = -q;
                q = q - *this;
                q = *this + q * scalar;
                q.normalize();
                return q;
        }

        //---------------------------------------------------------------------------------
        Matrix Quaternion::convert() const
        {
                // matrix
                Matrix matrix;

                float x2 = x * x;
                float y2 = y * y;
                float z2 = z * z;
                float w2 = w * w;

                // compute inverse square length
                float l2 = 1.0f / (x2 + y2 + z2 + w2);

                matrix.a[0][0] = (x2 - y2 - z2 + w2) * l2;
                matrix.a[1][1] = (-x2 + y2 - z2 + w2) * l2;
                matrix.a[2][2] = (-x2 - y2 + z2 + w2) * l2;

                float t1 = x * y;
                float t2 = z * w;

                matrix.a[1][0] = 2.0f * (t1 + t2) * l2;
                matrix.a[0][1] = 2.0f * (t1 - t2) * l2;

                t1 = x * z;
                t2 = y * w;

                matrix.a[2][0] = 2.0f * (t1 - t2) * l2;
                matrix.a[0][2] = 2.0f * (t1 + t2) * l2;

                t1 = y * z;
                t2 = x * w;

                matrix.a[2][1] = 2.0f * (t1 + t2) * l2;
                matrix.a[1][2] = 2.0f * (t1 - t2) * l2;

                matrix.a[3][3] = 1.0f;

                return matrix;
        }

        //---------------------------------------------------------------------------------
        Quaternion::operator float*()
        {
                return &x;
        }

        //---------------------------------------------------------------------------------
        Quaternion::operator const float*() const
        {
                return &x;
        }

        //---------------------------------------------------------------------------------
        Quaternion& Quaternion::operator +=(float scalar)
        {
                x += scalar;
                y += scalar;
                z += scalar;
                w += scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Quaternion& Quaternion::operator +=(const Quaternion& quaternion)
        {
                x += quaternion.x;
                y += quaternion.y;
                z += quaternion.z;
                w += quaternion.w;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Quaternion& Quaternion::operator -=(float scalar)
        {
                x -= scalar;
                y -= scalar;
                z -= scalar;
                w -= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Quaternion& Quaternion::operator -=(const Quaternion& quaternion)
        {
                x -= quaternion.x;
                y -= quaternion.y;
                z -= quaternion.z;
                w -= quaternion.w;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Quaternion& Quaternion::operator *=(float scalar)
        {
                x *= scalar;
                y *= scalar;
                z *= scalar;
                w *= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Quaternion& Quaternion::operator *=(const Quaternion& quaternion)
        {
                *this = (*this) * quaternion;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Quaternion& Quaternion::operator /=(float scalar)
        {
                x /= scalar;
                y /= scalar;
                z /= scalar;
                w /= scalar;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Quaternion& Quaternion::operator /=(const Quaternion& quaternion)
        {
                x /= quaternion.x;
                y /= quaternion.y;
                z /= quaternion.z;
                w /= quaternion.w;
                return *this;
        }

        //---------------------------------------------------------------------------------
        Vector2d operator +(const Vector2d& vector)
        {
                return vector;
        }

        //---------------------------------------------------------------------------------
        Vector2d operator +(const Vector2d& vector, float scalar)
        {
                return Vector2d(vector.x + scalar, vector.y + scalar);
        }

        //---------------------------------------------------------------------------------
        Vector2d operator +(const Vector2d& vector0, const Vector2d& vector1)
        {
                return Vector2d(vector0.x + vector1.x, vector0.y + vector1.y);
        }

        //---------------------------------------------------------------------------------
        Vector2d operator -(const Vector2d& vector)
        {
                return Vector2d(-vector.x, -vector.y);
        }

        //---------------------------------------------------------------------------------
        Vector2d operator -(const Vector2d& vector, float scalar)
        {
                return Vector2d(vector.x - scalar, vector.y - scalar);
        }

        //---------------------------------------------------------------------------------
        Vector2d operator -(const Vector2d& vector0, const Vector2d& vector1)
        {
                return Vector2d(vector0.x - vector1.x, vector0.y - vector1.y);
        }

        //---------------------------------------------------------------------------------
        Vector2d operator *(const Vector2d& vector, float scalar)
        {
                return Vector2d(vector.x * scalar, vector.y * scalar);
        }

        //---------------------------------------------------------------------------------
        Vector2d operator *(float scalar, const Vector2d& vector)
        {
                return Vector2d(vector.x * scalar, vector.y * scalar);
        }

        //---------------------------------------------------------------------------------
        Vector2d operator /(const Vector2d& vector, float scalar)
        {
                return Vector2d(vector.x / scalar, vector.y / scalar);
        }

        //---------------------------------------------------------------------------------
        Vector2d operator /(const Vector2d& vector0, const Vector2d& vector1)
        {
                return Vector2d(vector0.x / vector1.x, vector0.y / vector1.y);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator +(const Vector3d& vector)
        {
                return vector;
        }

        //---------------------------------------------------------------------------------
        Vector3d operator +(const Vector3d& vector, float scalar)
        {
                return Vector3d(vector.x + scalar, vector.y + scalar, vector.z + scalar);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator +(const Vector3d& vector0, const Vector3d& vector1)
        {
                return Vector3d(vector0.x + vector1.x,
                                vector0.y + vector1.y,
                                vector0.z + vector1.z);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator -(const Vector3d& vector)
        {
                return Vector3d(-vector.x, -vector.y, -vector.z);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator -(const Vector3d& vector, float scalar)
        {
                return Vector3d(vector.x - scalar, vector.y - scalar, vector.z - scalar);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator -(const Vector3d& vector0, const Vector3d& vector1)
        {
                return Vector3d(vector0.x - vector1.x,
                                vector0.y - vector1.y,
                                vector0.z - vector1.z);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator *(const Vector3d& vector, float scalar)
        {
                return Vector3d(vector.x * scalar, vector.y * scalar, vector.z * scalar);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator *(float scalar, const Vector3d& vector)
        {
                return Vector3d(vector.x * scalar, vector.y * scalar, vector.z * scalar);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator *(const Vector3d& vector, const Matrix& matrix)
        {
                Vector3d result(vector.x * matrix.a[0][0] + vector.y * matrix.a[1][0] +
                                vector.z * matrix.a[2][0] + matrix.a[3][0],
                                vector.x * matrix.a[0][1] + vector.y * matrix.a[1][1] +
                                vector.z * matrix.a[2][1] + matrix.a[3][1],
                                vector.x * matrix.a[0][2] + vector.y * matrix.a[1][2] +
                                vector.z * matrix.a[2][2] + matrix.a[3][2]);
                float w = vector.x * matrix.a[0][3] + vector.y * matrix.a[1][3] +
                          vector.z * matrix.a[2][3] + matrix.a[3][3];
                w = 1.0f / w;
                result *= w;

                return result;
        }

        //---------------------------------------------------------------------------------
        Vector3d operator /(const Vector3d& vector, float scalar)
        {
                return Vector3d(vector.x / scalar, vector.y / scalar, vector.z / scalar);
        }

        //---------------------------------------------------------------------------------
        Vector3d operator /(const Vector3d& vector0, const Vector3d& vector1)
        {
                return Vector3d(vector0.x / vector1.x,
                                vector0.y / vector1.y,
                                vector0.z / vector1.z);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator +(const Vector4d& vector)
        {
                return vector;
        }

        //---------------------------------------------------------------------------------
        Vector4d operator +(const Vector4d& vector, float scalar)
        {
                return Vector4d(vector.x + scalar, vector.y + scalar,
                                vector.z + scalar, vector.w + scalar);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator +(const Vector4d& vector0, const Vector4d& vector1)
        {
                return Vector4d(vector0.x + vector1.x, vector0.y + vector1.y,
                                vector0.z + vector1.z, vector0.w + vector1.w);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator -(const Vector4d& vector)
        {
                return Vector4d(-vector.x, -vector.y, -vector.z, -vector.w);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator -(const Vector4d& vector, float scalar)
        {
                return Vector4d(vector.x - scalar, vector.y - scalar,
                                vector.z - scalar, vector.w - scalar);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator -(const Vector4d& vector0, const Vector4d& vector1)
        {
                return Vector4d(vector0.x - vector1.x, vector0.y - vector1.y,
                                vector0.z - vector1.z, vector0.w - vector1.w);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator *(const Vector4d& vector, float scalar)
        {
                return Vector4d(vector.x * scalar, vector.y * scalar,
                                vector.z * scalar, vector.w * scalar);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator *(float scalar, const Vector4d& vector)
        {
                return Vector4d(vector.x * scalar, vector.y * scalar,
                                vector.z * scalar, vector.w * scalar);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator *(const Vector4d& vector, const Matrix& matrix)
        {
                return Vector4d(vector.x * matrix.a[0][0] + vector.y * matrix.a[1][0] +
                                vector.z * matrix.a[2][0] + vector.w * matrix.a[3][0],
                                vector.x * matrix.a[0][1] + vector.y * matrix.a[1][1] +
                                vector.z * matrix.a[2][1] + vector.w * matrix.a[3][1],
                                vector.x * matrix.a[0][2] + vector.y * matrix.a[1][2] +
                                vector.z * matrix.a[2][2] + vector.w * matrix.a[3][2],
                                vector.x * matrix.a[0][3] + vector.y * matrix.a[1][3] +
                                vector.z * matrix.a[2][3] + vector.w * matrix.a[3][3]);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator /(const Vector4d& vector, float scalar)
        {
                return Vector4d(vector.x / scalar, vector.y / scalar,
                                vector.z / scalar, vector.w / scalar);
        }

        //---------------------------------------------------------------------------------
        Vector4d operator /(const Vector4d& vector0, const Vector4d& vector1)
        {
                return Vector4d(vector0.x / vector1.x, vector0.y / vector1.y,
                                vector0.z / vector1.z, vector0.w / vector1.w);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator +(const Quaternion& quaternion)
        {
                return quaternion;
        }

        //---------------------------------------------------------------------------------
        Quaternion operator +(const Quaternion& quaternion, float scalar)
        {
                return Quaternion(quaternion.x + scalar, quaternion.y + scalar,
                                  quaternion.z + scalar, quaternion.w + scalar);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator +(const Quaternion& quaternion0, const Quaternion& quaternion1)
        {
                return Quaternion(quaternion0.x + quaternion1.x,
                                  quaternion0.y + quaternion1.y,
                                  quaternion0.z + quaternion1.z,
                                  quaternion0.w + quaternion1.w);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator -(const Quaternion& quaternion)
        {
                return Quaternion(-quaternion.x, -quaternion.y,
                                  -quaternion.z, -quaternion.w);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator -(const Quaternion& quaternion, float scalar)
        {
                return Quaternion(quaternion.x - scalar, quaternion.y - scalar,
                                  quaternion.z - scalar, quaternion.w - scalar);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator -(const Quaternion& quaternion0, const Quaternion& quaternion1)
        {
                return Quaternion(quaternion0.x - quaternion1.x,
                                  quaternion0.y - quaternion1.y,
                                  quaternion0.z - quaternion1.z,
                                  quaternion0.w - quaternion1.w);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator *(const Quaternion& quaternion, float scalar)
        {
                return Quaternion(quaternion.x * scalar, quaternion.y * scalar,
                                  quaternion.z * scalar, quaternion.w * scalar);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator *(float scalar, const Quaternion& quaternion)
        {
                return Quaternion(quaternion.x * scalar, quaternion.y * scalar,
                                  quaternion.z * scalar, quaternion.w * scalar);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator *(const Quaternion& quaternion0, const Quaternion& quaternion1)
        {
                Vector3d v0(quaternion0.x, quaternion0.y, quaternion0.z);
                Vector3d v1(quaternion1.x, quaternion1.y, quaternion1.z);
                Vector3d p = quaternion0.w * v1 + quaternion1.w * v0 + v0.cross(v1);
                return Quaternion(p, quaternion0.w * quaternion1.w - v0.dot(v1));
        }

        //---------------------------------------------------------------------------------
        Quaternion operator /(const Quaternion& quaternion, float scalar)
        {
                return Quaternion(quaternion.x / scalar, quaternion.y / scalar,
                                  quaternion.z / scalar, quaternion.w / scalar);
        }

        //---------------------------------------------------------------------------------
        Quaternion operator /(const Quaternion& quaternion0, const Quaternion& quaternion1)
        {
                return Quaternion(quaternion0.x / quaternion1.x,
                                  quaternion0.y / quaternion1.y,
                                  quaternion0.z / quaternion1.z,
                                  quaternion0.w / quaternion1.w);
        }

}
