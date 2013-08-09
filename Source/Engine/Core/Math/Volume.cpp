// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#include "Volume.h"
#include "Matrix.h"

namespace selene
{

        Volume::Volume(const Plane* planes, uint8_t numPlanes): numPlanes_(0)
        {
                define(planes, numPlanes);
        }
        Volume::~Volume() {}

        //------------------------------------------------------------------------------
        void Volume::define(const Plane* planes, uint8_t numPlanes)
        {
                // validate
                if(numPlanes == 0 || numPlanes > MAX_NUM_OF_PLANES || planes == nullptr)
                {
                        numPlanes_ = 0;
                        return;
                }

                numPlanes_ = numPlanes;
                for(uint8_t i = 0; i < numPlanes_; ++i)
                        planes_[i] = planes[i];
        }

        //------------------------------------------------------------------------------
        void Volume::define(const Matrix& viewProjectionMatrix)
        {
                const float* elements = static_cast<const float*>(viewProjectionMatrix);

                // set number of planes
                numPlanes_ = 6;

                // extract the frustum's right clipping plane and normalize it
                planes_[5].define(elements[ 3] - elements[ 0],
                                  elements[ 7] - elements[ 4],
                                  elements[11] - elements[ 8],
                                  elements[15] - elements[12]);
                planes_[5].normalize();

                // extract the frustum's left clipping plane and normalize it
                planes_[4].define(elements[ 3] + elements[ 0],
                                  elements[ 7] + elements[ 4],
                                  elements[11] + elements[ 8],
                                  elements[15] + elements[12]);
                planes_[4].normalize();

                // extract the frustum's bottom clipping plane and normalize it
                planes_[3].define(elements[ 3] + elements[ 1],
                                  elements[ 7] + elements[ 5],
                                  elements[11] + elements[ 9],
                                  elements[15] + elements[13]);
                planes_[3].normalize();

                // extract the frustum's top clipping plane and normalize it
                planes_[2].define(elements[ 3] - elements[ 1],
                                  elements[ 7] - elements[ 5],
                                  elements[11] - elements[ 9],
                                  elements[15] - elements[13]);
                planes_[2].normalize();

                // extract the frustum's far clipping plane and normalize it
                planes_[1].define(elements[ 3] - elements[ 2],
                                  elements[ 7] - elements[ 6],
                                  elements[11] - elements[10],
                                  elements[15] - elements[14]);
                planes_[1].normalize();

                // extract the frustum's near clipping plane and normalize it
                planes_[0].define(elements[ 3] + elements[ 2],
                                  elements[ 7] + elements[ 6],
                                  elements[11] + elements[10],
                                  elements[15] + elements[14]);
                planes_[0].normalize();
        }

        //------------------------------------------------------------------------------
        uint8_t Volume::getNumPlanes() const
        {
                return numPlanes_;
        }

        //------------------------------------------------------------------------------
        const Plane* Volume::getPlanes() const
        {
                return planes_;
        }

}
