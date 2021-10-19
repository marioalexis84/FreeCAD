/***************************************************************************
 *   Copyright (c) 2021 Mario Passaglia                                    *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"

#include "ImageCanny.h"

#include "opencv2/imgproc.hpp"

using namespace Image;

const char* ImageCanny::NormEnum[] = {"L1", "L2", NULL};
const App::PropertyIntegerConstraint::Constraints ConstraintApertureSize = {3, 7, 2};

PROPERTY_SOURCE(Image::ImageCanny, Image::ImageFilter)

ImageCanny::ImageCanny()
{
    ADD_PROPERTY_TYPE(Threshold1, (0), "ImageCanny", App::Prop_None,
        "First threshold");
    ADD_PROPERTY_TYPE(Threshold2, (0), "ImageCanny", App::Prop_None,
        "Second threshold");
    ApertureSize.setConstraints(&ConstraintApertureSize);
    ADD_PROPERTY_TYPE(ApertureSize, (3), "ImageCanny", App::Prop_None,
        "Sobel operator aperture size");
    Norm.setEnums(NormEnum);
    ADD_PROPERTY_TYPE(Norm, (0L), "ImageCanny", App::Prop_None,
        "Norm used to calculate the image gradient");
}

ImageCanny::~ImageCanny()
{
}

void ImageCanny::setCanny(const double& thresh1, const double& thresh2,
    const int& size, const int& norm)
{
    cv::Mat tmpMat;
    cv::Mat tmpSrc = getLinkMat(&SourceImage);

    cv::Canny(tmpSrc, tmpMat, thresh1, thresh2, size, bool(norm));

    MatImage.setValue(tmpMat);
}

App::DocumentObjectExecReturn* ImageCanny::execute()
{
    if (isEmpty())
        return nullptr;

    App::DocumentObjectExecReturn* ret = ImageObject::execute();
    return ret;
}

void ImageCanny::onChanged(const App::Property* prop)
{
    if (prop == &Threshold1 || prop == &Threshold2 || prop == &ApertureSize || prop == &Norm) {
        if (!linkIsEmpty(&SourceImage)) {
            double thresh1 = Threshold1.getValue();
            double thresh2 = Threshold2.getValue();
            int size = ApertureSize.getValue();
            int norm = Norm.getValue();

            setCanny(thresh1, thresh2, size, norm);
        }
    }

    ImageObject::onChanged(prop);
}
//ImageCanny::

//**************************************************************************
