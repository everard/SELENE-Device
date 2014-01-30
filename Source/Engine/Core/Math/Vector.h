// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef VECTOR_H
#define VECTOR_H

#include "../Macros/Macros.h"

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
                explicit Vector2d(float x_ = 0.0f, float y_ = 0.0f);
                Vector2d(const Vector2d&) = default;
                ~Vector2d();
                Vector2d& operator =(const Vector2d&) = default;

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

                // Operators
                operator float*();
                operator const float*() const;

                Vector2d& operator +=(float scalar);
                Vector2d& operator +=(const Vector2d& vector);

                Vector2d& operator -=(float scalar);
                Vector2d& operator -=(const Vector2d& vector);

                Vector2d& operator *=(float scalar);
                Vector2d& operator *=(const Vector2d& vector);

                Vector2d& operator /=(float scalar);
                Vector2d& operator /=(const Vector2d& vector);

                bool operator ==(const Vector2d& vector);

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
                explicit Vector3d(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f);
                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] vector holds x and y coordinates
                 * \param[in] z_ z coordinate of vector
                 */
                explicit Vector3d(const Vector2d& vector, float z_ = 0.0f);
                Vector3d(const Vector3d&) = default;
                ~Vector3d();
                Vector3d& operator =(const Vector3d&) = default;

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] x_ x coordinate of vector
                 * \param[in] y_ y coordinate of vector
                 * \param[in] z_ z coordinate of vector
                 */
                void define(float x_, float y_, float z_);

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] vector holds x and y coordinates
                 * \param[in] z_ z coordinate of vector
                 */
                void define(const Vector2d& vector, float z_);

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
                 * \brief Computes cross product.
                 * \param[in] vector another vector
                 * \return cross product of two vectors
                 */
                Vector3d cross(const Vector3d& vector) const;

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

                // Operators
                operator float*();
                operator const float*() const;

                Vector3d& operator +=(float scalar);
                Vector3d& operator +=(const Vector3d& vector);

                Vector3d& operator -=(float scalar);
                Vector3d& operator -=(const Vector3d& vector);

                Vector3d& operator *=(float scalar);
                Vector3d& operator *=(const Matrix& matrix);

                Vector3d& operator /=(float scalar);
                Vector3d& operator /=(const Vector3d& vector);

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
                explicit Vector4d(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f, float w_ = 0.0f);
                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] vector holds x and y coordinates
                 * \param[in] z_ z coordinate of vector
                 * \param[in] w_ w coordinate of vector
                 */
                explicit Vector4d(const Vector2d& vector, float z_ = 0.0f, float w_ = 0.0f);
                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] vector0 holds x and y coordinates
                 * \param[in] vector1 holds z and w coordinates
                 */
                explicit Vector4d(const Vector2d& vector0, const Vector2d& vector1);
                /**
                 * \brief Constructs vector with given coordinates.
                 * \param[in] vector holds x, y and z coordinates
                 * \param[in] w_ w coordinate of vector
                 */
                explicit Vector4d(const Vector3d& vector, float w_ = 0.0f);
                Vector4d(const Vector4d&) = default;
                ~Vector4d();
                Vector4d& operator =(const Vector4d&) = default;

                /**
                 * \brief Defines vector with given coordinates.
                 * \param[in] x_ x coordinate of vector
                 * \param[in] y_ y coordinate of vector
                 * \param[in] z_ z coordinate of vector
                 * \param[in] w_ w coordinate of vector
                 */
                void define(float x_, float y_, float z_, float w_);

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
                 * \brief Defines vector with given scalar (makes x, y, z and w coordinates equal to scalar).
                 * \param[in] scalar x, y, z and w coordinates
                 */
                void define(float scalar);

                // Operators
                operator float*();
                operator const float*() const;

                Vector4d& operator +=(float scalar);
                Vector4d& operator +=(const Vector4d& vector);

                Vector4d& operator -=(float scalar);
                Vector4d& operator -=(const Vector4d& vector);

                Vector4d& operator *=(float scalar);
                Vector4d& operator *=(const Vector4d& vector);

                Vector4d& operator /=(float scalar);
                Vector4d& operator /=(const Vector4d& vector);

                bool operator ==(const Vector4d& vector);

        };

        /**
         * Represents quaternion.
         */
        class Quaternion
        {
        public:
                float x, y, z, w;

                /**
                 * \brief Constructs quaternion with given coordinates.
                 * \param[in] x_ x coordinate of quaternion
                 * \param[in] y_ y coordinate of quaternion
                 * \param[in] z_ z coordinate of quaternion
                 * \param[in] w_ w coordinate of quaternion
                 */
                explicit Quaternion(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f, float w_ = 1.0f);
                /**
                 * \brief Constructs quaternion with given coordinates.
                 * \param[in] vector holds x and y coordinates
                 * \param[in] z_ z coordinate of quaternion
                 * \param[in] w_ w coordinate of quaternion
                 */
                explicit Quaternion(const Vector2d& vector, float z_ = 0.0f, float w_ = 0.0f);
                /**
                 * \brief Constructs quaternion with given coordinates.
                 * \param[in] vector0 holds x and y coordinates
                 * \param[in] vector1 holds z and w coordinates
                 */
                explicit Quaternion(const Vector2d& vector0, const Vector2d& vector1);
                /**
                 * \brief Constructs quaternion with given coordinates.
                 * \param[in] vector holds x, y and z coordinates
                 * \param[in] w_ w coordinate of quaternion
                 */
                explicit Quaternion(const Vector3d& vector, float w_ = 0.0f);
                Quaternion(const Quaternion&) = default;
                ~Quaternion();
                Quaternion& operator =(const Quaternion&) = default;

                /**
                 * \brief Defines quaternion with given coordinates.
                 * \param[in] x_ x coordinate of quaternion
                 * \param[in] y_ y coordinate of quaternion
                 * \param[in] z_ z coordinate of quaternion
                 * \param[in] w_ w coordinate of quaternion
                 */
                void define(float x_, float y_, float z_, float w_);

                /**
                 * \brief Defines quaternion with given coordinates.
                 * \param[in] vector holds x and y coordinates
                 * \param[in] z_ z coordinate of quaternion
                 * \param[in] w_ w coordinate of quaternion
                 */
                void define(const Vector2d& vector, float z_, float w_);

                /**
                 * \brief Defines quaternion with given coordinates.
                 * \param[in] vector0 holds x and y coordinates
                 * \param[in] vector1 holds z and w coordinates
                 */
                void define(const Vector2d& vector0, const Vector2d& vector1);

                /**
                 * \brief Defines quaternion with given coordinates.
                 * \param[in] vector holds x, y and z coordinates
                 * \param[in] w_ w coordinate of quaternion
                 */
                void define(const Vector3d& vector, float w_);

                /**
                 * \brief Defines quaternion with given scalar (makes x, y, z and w coordinates equal to scalar).
                 * \param[in] scalar x, y, z and w coordinates
                 */
                void define(float scalar);

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
                 * \param[in] vector vector, which will be rotated
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

                // Operators
                operator float*();
                operator const float*() const;

                Quaternion& operator +=(float scalar);
                Quaternion& operator +=(const Quaternion& quaternion);

                Quaternion& operator -=(float scalar);
                Quaternion& operator -=(const Quaternion& quaternion);

                Quaternion& operator *=(float scalar);
                Quaternion& operator *=(const Quaternion& quaternion);

                Quaternion& operator /=(float scalar);
                Quaternion& operator /=(const Quaternion& quaternion);

        };

        // Vector2d operators
        Vector2d operator +(const Vector2d& vector);
        Vector2d operator +(const Vector2d& vector, float scalar);
        Vector2d operator +(const Vector2d& vector0, const Vector2d& vector1);

        Vector2d operator -(const Vector2d& vector);
        Vector2d operator -(const Vector2d& vector, float scalar);
        Vector2d operator -(const Vector2d& vector0, const Vector2d& vector1);

        Vector2d operator *(const Vector2d& vector, float scalar);
        Vector2d operator *(float scalar, const Vector2d& vector);

        Vector2d operator /(const Vector2d& vector, float scalar);
        Vector2d operator /(const Vector2d& vector0, const Vector2d& vector1);

        // Vector3d operators
        Vector3d operator +(const Vector3d& vector);
        Vector3d operator +(const Vector3d& vector, float scalar);
        Vector3d operator +(const Vector3d& vector0, const Vector3d& vector1);

        Vector3d operator -(const Vector3d& vector);
        Vector3d operator -(const Vector3d& vector, float scalar);
        Vector3d operator -(const Vector3d& vector0, const Vector3d& vector1);

        Vector3d operator *(const Vector3d& vector, float scalar);
        Vector3d operator *(float scalar, const Vector3d& vector);
        Vector3d operator *(const Vector3d& vector, const Matrix& matrix);

        Vector3d operator /(const Vector3d& vector, float scalar);
        Vector3d operator /(const Vector3d& vector0, const Vector3d& vector1);

        // Vector4d operators
        Vector4d operator +(const Vector4d& vector);
        Vector4d operator +(const Vector4d& vector, float scalar);
        Vector4d operator +(const Vector4d& vector0, const Vector4d& vector1);

        Vector4d operator -(const Vector4d& vector);
        Vector4d operator -(const Vector4d& vector, float scalar);
        Vector4d operator -(const Vector4d& vector0, const Vector4d& vector1);

        Vector4d operator *(const Vector4d& vector, float scalar);
        Vector4d operator *(float scalar, const Vector4d& vector);
        Vector4d operator *(const Vector4d& vector, const Matrix& matrix);

        Vector4d operator /(const Vector4d& vector, float scalar);
        Vector4d operator /(const Vector4d& vector0, const Vector4d& vector1);

        // Quaternion operators
        Quaternion operator +(const Quaternion& quaternion);
        Quaternion operator +(const Quaternion& quaternion, float scalar);
        Quaternion operator +(const Quaternion& quaternion0, const Quaternion& quaternion1);

        Quaternion operator -(const Quaternion& quaternion);
        Quaternion operator -(const Quaternion& quaternion, float scalar);
        Quaternion operator -(const Quaternion& quaternion0, const Quaternion& quaternion1);

        Quaternion operator *(const Quaternion& quaternion, float scalar);
        Quaternion operator *(float scalar, const Quaternion& quaternion);
        Quaternion operator *(const Quaternion& quaternion0, const Quaternion& quaternion1);

        Quaternion operator /(const Quaternion& quaternion, float scalar);
        Quaternion operator /(const Quaternion& quaternion0, const Quaternion& quaternion1);

        /**
         * @}
         */

}

#endif
