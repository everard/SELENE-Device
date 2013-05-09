// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"

namespace selene
{

        /**
         * \addtogroup Math
         * @{
         */

        /**
         * Represents 4x4 matrix.
         */
        class Matrix
        {
        public:
                float a[4][4];

                /**
                 * \brief Constructs matrix with given elements.
                 * \param[in] a11 element of the matrix
                 * \param[in] a12 element of the matrix
                 * \param[in] a13 element of the matrix
                 * \param[in] a14 element of the matrix
                 * \param[in] a21 element of the matrix
                 * \param[in] a22 element of the matrix
                 * \param[in] a23 element of the matrix
                 * \param[in] a24 element of the matrix
                 * \param[in] a31 element of the matrix
                 * \param[in] a32 element of the matrix
                 * \param[in] a33 element of the matrix
                 * \param[in] a34 element of the matrix
                 * \param[in] a41 element of the matrix
                 * \param[in] a42 element of the matrix
                 * \param[in] a43 element of the matrix
                 * \param[in] a44 element of the matrix
                 */
                Matrix(float a11 = 0.0f, float a12 = 0.0f, float a13 = 0.0f, float a14 = 0.0f,
                       float a21 = 0.0f, float a22 = 0.0f, float a23 = 0.0f, float a24 = 0.0f,
                       float a31 = 0.0f, float a32 = 0.0f, float a33 = 0.0f, float a34 = 0.0f,
                       float a41 = 0.0f, float a42 = 0.0f, float a43 = 0.0f, float a44 = 0.0f);
                ~Matrix();

                /**
                 * \brief Defines matrix with given elements.
                 * \param[in] a11 element of the matrix
                 * \param[in] a12 element of the matrix
                 * \param[in] a13 element of the matrix
                 * \param[in] a14 element of the matrix
                 * \param[in] a21 element of the matrix
                 * \param[in] a22 element of the matrix
                 * \param[in] a23 element of the matrix
                 * \param[in] a24 element of the matrix
                 * \param[in] a31 element of the matrix
                 * \param[in] a32 element of the matrix
                 * \param[in] a33 element of the matrix
                 * \param[in] a34 element of the matrix
                 * \param[in] a41 element of the matrix
                 * \param[in] a42 element of the matrix
                 * \param[in] a43 element of the matrix
                 * \param[in] a44 element of the matrix
                 */
                void define(float a11, float a12, float a13, float a14,
                            float a21, float a22, float a23, float a24,
                            float a31, float a32, float a33, float a34,
                            float a41, float a42, float a43, float a44);

                /**
                 * \brief Creates identity matrix.
                 */
                void identity();

                /**
                 * \brief Creates translation matrix.
                 * \param[in] translationVector translation vector
                 */
                void translation(const Vector3d& translationVector);

                /**
                 * \brief Creates scale matrix.
                 * \param[in] scaleVector amount of scale for each axis
                 */
                void scale(const Vector3d& scaleVector);

                /**
                 * \brief Creates rotation matrix around X CCW.
                 * \param[in] angle rotation angle in radians
                 */
                void rotationX(float angle);

                /**
                 * \brief Creates rotation matrix around Y CCW.
                 * \param[in] angle rotation angle in radians
                 */
                void rotationY(float angle);

                /**
                 * \brief Creates rotation matrix around Z CCW.
                 * \param[in] angle rotation angle in radians
                 */
                void rotationZ(float angle);

                /**
                 * \brief Creates yaw, pitch, roll rotation matrix.
                 * \param[in] rotationVector rotation angles in radians for each axis
                 */
                void yawPitchRoll(const Vector3d& rotationVector);

                /**
                 * \brief Creates perspective matrix.
                 * \param[in] fov field of view in grad
                 * \param[in] aspect aspect ratio
                 * \param[in] zn distance to the near clipping plane
                 * \param[in] zf distance to the far clipping plane
                 */
                void perspective(float fov, float aspect, float zn, float zf);

                /**
                 * \brief Creates look at matrix.
                 * \param[in] eyePosition position of the eye
                 * \param[in] targetPosition position of the target
                 * \param[in] upVector up vector
                 */
                void lookAt(const Vector3d& eyePosition,
                            const Vector3d& targetPosition,
                            const Vector3d& upVector);

                /**
                 * \brief Inverts matrix.
                 * \return true if operation succeeded
                 */
                bool invert();

                /**
                 * \brief Transposes matrix.
                 */
                void transpose();

                // Operators
                operator float*();
                operator const float*() const;

        };

        // Matrix operators
        Matrix operator +(const Matrix& matrix);
        Matrix operator +(const Matrix& matrix, float scalar);
        Matrix operator +(const Matrix& matrix0, const Matrix& matrix1);

        Matrix operator -(const Matrix& matrix);
        Matrix operator -(const Matrix& matrix, float scalar);
        Matrix operator -(const Matrix& matrix0, const Matrix& matrix1);

        Matrix operator *(const Matrix& matrix0, const Matrix& matrix1);

        /**
         * @}
         */

}

#endif
