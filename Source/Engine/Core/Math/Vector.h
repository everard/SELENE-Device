// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef VECTOR_H
#define VECTOR_H

#include "../Macros/Macros.h"
#include <cstdlib>
#include <cmath>

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        // Forward declaration of classes
        class Matrix;

        /**
         * Represents vector in 2D space.
         */
        class Vector2d
        {
        public:
                float x, y;

                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] x_ x coordinate of vector
                 * \param[in] y_ y coordinate of vector
                 */
                Vector2d(float x_ = 0.0f, float y_ = 0.0f);
                ~Vector2d();

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] x_ x coordinate of vector
                 * \param[in] y_ y coordinate of vector
                 */
                void define(float x_, float y_);

                /**
                 * \brief Defines vector with given scalar (makes both x and y coordinates equal to scalar).
                 * \param[in] scalar x and y coordinates
                 */
                void define(float scalar);

                /**
                 * \brief Computes length.
                 * \return length of the vector
                 */
                float length() const;

                /**
                 * \brief Computes dot product.
                 * \param[in] vector another vector
                 * \return dot product of two vectors
                 */
                float dot(const Vector2d& vector) const;

                /**
                 * \brief Normalizes vector.
                 */
                void normalize();

                /**
                 * \brief Converts to float*.
                 */
                operator float*();

                /**
                 * \brief Converts to const float*.
                 */
                operator const float*() const;

                /**
                 * \brief Assignes vector.
                 * \param[in] vector another vector which will be assigned to current
                 * \return reference to the vector
                 */
                Vector2d& operator =(const Vector2d& vector);

                /**
                 * \brief Unary "+" operator.
                 * \param[in] vector initial vector
                 * \return result of unary "+" operation
                 */
                friend Vector2d operator +(const Vector2d& vector);

                /**
                 * \brief Computes sum of two vectors.
                 * \param[in] vector0 the first vector
                 * \param[in] vector1 the second vector
                 * \return sum of two vectors (vector0 + vector1)
                 */
                friend Vector2d operator +(const Vector2d& vector0, const Vector2d& vector1);

                /**
                 * \brief Unary "-" operator.
                 * \param[in] vector initial vector
                 * \return result of unary "-" operation
                 */
                friend Vector2d operator -(const Vector2d& vector);

                /**
                 * \brief Subtracts one vector from another.
                 * \param[in] vector0 the first vector
                 * \param[in] vector1 the second vector
                 * \return difference of two vectors (vector0 - vector1)
                 */
                friend Vector2d operator -(const Vector2d& vector0, const Vector2d& vector1);

                /**
                 * \brief Multiplies vector by scalar.
                 * \param[in] vector vector
                 * \param[in] scalar scalar
                 * \return product (vector * scalar)
                 */
                friend Vector2d operator *(const Vector2d& vector, float scalar);

                /**
                 * \brief Multiplies vector by scalar.
                 * \param[in] scalar scalar
                 * \param[in] vector vector
                 * \return product (scalar * vector)
                 */
                friend Vector2d operator *(float scalar, const Vector2d& vector);

                /**
                 * \brief Divides vector by scalar.
                 * \param[in] vector vector
                 * \param[in] scalar scalar
                 * \return quotient (vector / scalar)
                 */
                friend Vector2d operator /(const Vector2d& vector, float scalar);

                /**
                 * \brief Divides two vectors.
                 * \param[in] vector0 the first vector
                 * \param[in] vector1 the second vector
                 * \return quotient (vector0 / vector1)
                 */
                friend Vector2d operator /(const Vector2d& vector0, const Vector2d& vector1);

                /**
                 * \brief Computes sum of current vector and another.
                 * \param[in] vector another vector which will be added to current
                 * \return reference to the vector
                 */
                Vector2d& operator +=(const Vector2d& vector);

                /**
                 * \brief Subtracts vector from current vector.
                 * \param[in] vector another vector which will be subtracted from current
                 * \return reference to the vector
                 */
                Vector2d& operator -=(const Vector2d& vector);

                /**
                 * \brief Multiplies current vector by scalar.
                 * \param[in] scalar scalar
                 * \return reference to the vector
                 */
                Vector2d& operator *=(float scalar);

        };

        /**
         * Represents vector in 3D space.
         */
        class Vector3d
        {
        public:
                float x, y, z;

                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] x_ x coordinate of vector
                 * \param[in] y_ y coordinate of vector
                 * \param[in] z_ z coordinate of vector
                 */
                Vector3d(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f);
                ~Vector3d();

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] x_ x coordinate of vector
                 * \param[in] y_ y coordinate of vector
                 * \param[in] z_ z coordinate of vector
                 */
                void define(float x_, float y_, float z_);

                /**
                 * \brief Defines vector with given scalar (makes x, y and z coordinates equal to scalar).
                 * \param[in] scalar x, y and z coordinates
                 */
                void define(float scalar);

                /**
                 * \brief Computes length.
                 * \return length of vector
                 */
                float length() const;

                /**
                 * \brief Computes dot product.
                 * \param[in] vector another vector
                 * \return dot product of two vectors
                 */
                float dot(const Vector3d& vector) const;

                /**
                 * \brief Normalizes vector.
                 */
                void normalize();

                /**
                 * \brief Interpolates vector.
                 *
                 * Interpolation is linear, and it is performed between current vector (first interpolation point)
                 * and specified second vector (second interpolation point) with given interpolation amount.
                 * \param[in] vector second interpolation point
                 * \param[in] scalar interpolation amount (float in [0; 1] range)
                 * \return result of interpolation
                 */
                Vector3d lerp(const Vector3d& vector, float scalar) const;

                /**
                 * \brief Scales vector.
                 * \param[in] vector specifies amount of scale for each component of initial vector
                 * \return scaled vector
                 */
                Vector3d scale(const Vector3d& vector) const;

                /**
                 * \brief Applies transformation.
                 * \param[in] matrix transformation matrix
                 */
                void transform(const Matrix& matrix);

                /**
                 * \brief Converts to float*.
                 */
                operator float*();

                /**
                 * \brief Converts to const float*.
                 */
                operator const float*() const;

                /**
                 * \brief Assignes vector.
                 * \param[in] vector another vector which will be assigned to current
                 * \return reference to the vector
                 */
                Vector3d& operator =(const Vector3d& vector);

                /**
                 * \brief Unary "+" operator.
                 * \param[in] vector initial vector
                 * \return result of unary "+" operation
                 */
                friend Vector3d operator +(const Vector3d& vector);

                /**
                 * \brief Computes sum of vector and scalar.
                 * \param[in] vector vector
                 * \param[in] scalar scalar
                 * \return sum of vector and scalar (vector + scalar)
                 */
                friend Vector3d operator +(const Vector3d& vector, float scalar);

                /**
                 * \brief Computes sum of two vectors.
                 * \param[in] vector0 the first vector
                 * \param[in] vector1 the second vector
                 * \return sum of two vectors (vector0 + vector1)
                 */
                friend Vector3d operator +(const Vector3d& vector0, const Vector3d& vector1);

                /**
                 * \brief Unary "-" operator.
                 * \param[in] vector initial vector
                 * \return result of unary "-" operation
                 */
                friend Vector3d operator -(const Vector3d& vector);

                /**
                 * \brief Subtracts one vector from another.
                 * \param[in] vector0 the first vector
                 * \param[in] vector1 the second vector
                 * \return difference of two vectors (vector0 - vector1)
                 */
                friend Vector3d operator -(const Vector3d& vector0, const Vector3d& vector1);

                /**
                 * \brief Subtracts a scalar from vector.
                 * \param[in] vector vector
                 * \param[in] scalar scalar
                 * \return difference (vector - scalar)
                 */
                friend Vector3d operator -(const Vector3d& vector, float scalar);

                /**
                 * \brief Computes cross product.
                 * \param[in] vector0 the first vector
                 * \param[in] vector1 the second vector
                 * \return cross product of two vectors
                 */
                friend Vector3d operator *(const Vector3d& vector0, const Vector3d& vector1);

                /**
                 * \brief Multiplies vector by scalar.
                 * \param[in] vector vector
                 * \param[in] scalar scalar
                 * \return product (vector * scalar)
                 */
                friend Vector3d operator *(const Vector3d& vector, float scalar);

                /**
                 * \brief Multiplies vector by scalar.
                 * \param[in] scalar scalar
                 * \param[in] vector vector
                 * \return product (scalar * vector)
                 */
                friend Vector3d operator *(float scalar, const Vector3d& vector);

                /**
                 * \brief Multiplies vector by matrix.
                 * \param[in] vector vector
                 * \param[in] matrix matrix
                 * \return product ((vector.xyz, 1.0) * matrix)
                 */
                friend Vector3d operator *(const Vector3d& vector, const Matrix& matrix);

                /**
                 * \brief Divides vector by scalar.
                 * \param[in] vector vector
                 * \param[in] scalar scalar
                 * \return quotient (vector / scalar)
                 */
                friend Vector3d operator /(const Vector3d& vector, float scalar);

                /**
                 * \brief Divides two vectors.
                 * \param[in] vector0 the first vector
                 * \param[in] vector1 the second vector
                 * \return quotient (vector0 / vector1)
                 */
                friend Vector3d operator /(const Vector3d& vector0, const Vector3d& vector1);

                /**
                 * \brief Computes sum of current vector and another.
                 * \param[in] vector another vector which will be added to current
                 * \return reference to the vector
                 */
                Vector3d& operator +=(const Vector3d& vector);

                /**
                 * \brief Computes sum of current vector and scalar.
                 * \param[in] scalar scalar
                 * \return reference to the vector
                 */
                Vector3d& operator +=(float scalar);

                /**
                 * \brief Subtracts vector from current vector.
                 * \param[in] vector another vector which will be subtracted from current
                 * \return reference to the vector
                 */
                Vector3d& operator -=(const Vector3d& vector);

                /**
                 * \brief Subtracts scalar from current vector.
                 * \param[in] scalar scalar
                 * \return reference to the vector
                 */
                Vector3d& operator -=(float scalar);

                /**
                 * \brief Multiplies current vector by scalar.
                 * \param[in] scalar scalar
                 * \return reference to the vector
                 */
                Vector3d& operator *=(float scalar);

                /**
                 * \brief Compares two vectors.
                 * \param[in] vector another vector which will be compared to current
                 * \return true if all components of vectors are almost equal, i.e. for each component:
                 * |component0 - component1| < EPSILON
                 */
                bool operator ==(const Vector3d& vector);

        };

        /**
         * Represents vector in 4D space.
         */
        class Vector4d
        {
        public:
                float x, y, z, w;

                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] x_ x coordinate of vector
                 * \param[in] y_ y coordinate of vector
                 * \param[in] z_ z coordinate of vector
                 * \param[in] w_ w coordinate of vector
                 */
                Vector4d(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f, float w_ = 0.0f);
                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] vector holds x and y coordinates
                 * \param[in] z_ z coordinate of vector
                 * \param[in] w_ w coordinate of vector
                 */
                Vector4d(const Vector2d& vector, float z_ = 0.0f, float w_ = 0.0f);
                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] vector0 holds x and y coordinates
                 * \param[in] vector1 holds z and w coordinates
                 */
                Vector4d(const Vector2d& vector0, const Vector2d& vector1);
                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] vector holds x, y and z coordinates
                 * \param[in] w_ w coordinate of vector
                 */
                Vector4d(const Vector3d& vector, float w_ = 0.0f);
                ~Vector4d();

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] x_ x coordinate of vector
                 * \param[in] y_ y coordinate of vector
                 * \param[in] z_ z coordinate of vector
                 * \param[in] w_ w coordinate of vector
                 */
                void define(float x_, float y_, float z_, float w_);

                /**
                 * \brief Defines vector with given scalar (makes x, y, z and w coordinates equal to scalar).
                 * \param[in] scalar x, y, z and w coordinates
                 */
                void define(float scalar);

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] vector holds x and y coordinates
                 * \param[in] z_ z coordinate of vector
                 * \param[in] w_ w coordinate of vector
                 */
                void define(const Vector2d& vector, float z_, float w_);

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] vector0 holds x and y coordinates
                 * \param[in] vector1 holds z and w coordinates
                 */
                void define(const Vector2d& vector0, const Vector2d& vector1);

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] vector holds x, y and z coordinates
                 * \param[in] w_ w coordinate of vector
                 */
                void define(const Vector3d& vector, float w_);

                /**
                 * \brief Assignes vector.
                 * \param[in] vector another vector which will be assigned to current
                 * \return reference to the vector
                 */
                Vector4d& operator =(const Vector4d& vector);

                /**
                 * \brief Converts to float*.
                 */
                operator float*();

                /**
                 * \brief Converts to const float*.
                 */
                operator const float*() const;

        };

        /**
         * Represents quaternion.
         */
        class Quaternion: public Vector4d
        {
        public:
                /**
                 * \brief Constructs quaternion with given coordinates.
                 * \param[in] x_ x coordinate of quaternion
                 * \param[in] y_ y coordinate of quaternion
                 * \param[in] z_ z coordinate of quaternion
                 * \param[in] w_ w coordinate of quaternion
                 */
                Quaternion(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f, float w_ = 1.0f);
                /**
                 * \brief Constructs quaternion with given coordinates.
                 * \param[in] vector holds x, y and z coordinates
                 * \param[in] w_ w coordinate of quaternion
                 */
                Quaternion(const Vector3d& vector, float w_ = 0.0f);
                ~Quaternion();

                /**
                 * \brief Computes inner product.
                 * \param[in] quaternion another quaternion
                 * \return inner product of two quaternions
                 */
                float inner(const Quaternion& quaternion) const;

                /**
                 * \brief Returns norm of the quaternion.
                 * \return norm of the quaternion
                 */
                float norm() const;

                /**
                 * \brief Normalizes quaternion.
                 */
                void normalize();

                /**
                 * \brief Returns conjugated quaternion.
                 * \return conjugated quaternion
                 */
                Quaternion conjugate() const;

                /**
                 * \brief Rotates given vector (quaternion must be normalized).
                 * \param[in] vector vector which will be rotated
                 * \return result of rotation
                 */
                Vector3d rotate(const Vector3d& vector) const;

                /**
                 * \brief Interpolates quaternion.
                 *
                 * Interpolation is linear, and it is performed between current quaternion (first interpolation point)
                 * and specified second quaternion (second interpolation point) with given interpolation amount.
                 * \param[in] quaternion second interpolation point
                 * \param[in] scalar interpolation amount (float in [0; 1] range)
                 * \return result of interpolation
                 */
                Quaternion lerp(const Quaternion& quaternion, float scalar) const;

                /**
                 * \brief Converts quaternion to matrix.
                 * \return corresponding rotation matrix
                 */
                Matrix convert() const;

                /**
                 * \brief Converts to float*.
                 */
                operator float*();

                /**
                 * \brief Converts to const float*.
                 */
                operator const float*() const;

                /**
                 * \brief Unary "-" operator.
                 * \param[in] quaternion initial quaternion
                 * \return result of unary "-" operation
                 */
                friend Quaternion operator -(const Quaternion& quaternion);

                /**
                 * \brief Subtracts one quaternion from another.
                 * \param[in] quaternion0 the first quaternion
                 * \param[in] quaternion1 the second quaternion
                 * \return difference of two quaternions (quaternion0 - quaternion1)
                 */
                friend Quaternion operator -(const Quaternion& quaternion0,
                                             const Quaternion& quaternion1);

                /**
                 * \brief Computes sum of two quaternions.
                 * \param[in] quaternion0 the first quaternion
                 * \param[in] quaternion1 the second quaternion
                 * \return sum of two quaternions (quaternion0 + quaternion1)
                 */
                friend Quaternion operator +(const Quaternion& quaternion0,
                                             const Quaternion& quaternion1);

                /**
                 * \brief Computes cross product.
                 * \param[in] quaternion0 the first quaternion
                 * \param[in] quaternion1 the second quaternion
                 * \return cross product of two quaternions
                 */
                friend Quaternion operator *(const Quaternion& quaternion0,
                                             const Quaternion& quaternion1);

                /**
                 * \brief Multiplies quaternion by scalar.
                 * \param[in] quaternion quaternion
                 * \param[in] scalar scalar
                 * \return product (quaternion * scalar)
                 */
                friend Quaternion operator *(const Quaternion& quaternion, float scalar);

                /**
                 * \brief Multiplies quaternion by scalar.
                 * \param[in] scalar scalar
                 * \param[in] quaternion quaternion
                 * \return product (scalar * quaternion)
                 */
                friend Quaternion operator *(float scalar, const Quaternion& quaternion);

                /**
                 * \brief Multiplies current quaternion by scalar.
                 * \param[in] scalar scalar
                 * \return reference to the quaternion
                 */
                Quaternion& operator *=(float scalar);

        };

        /**
         * @}
         */

}

#endif
