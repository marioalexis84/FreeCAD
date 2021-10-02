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

#include "ImageFilter.h"

#include "opencv2/imgproc.hpp"

using namespace Image;

template<typename T>
std::vector<std::string> mapEnum(std::map<std::string, T>& m)
{
    std::vector<std::string> strVec;
    for (const auto& p: m) {
        strVec.push_back(p.first);
    }
    return strVec;
}

//**************************************************************************
// ImageFilter
//**************************************************************************

PROPERTY_SOURCE(Image::ImageFilter, App::ImageObject)

ImageFilter::ImageFilter()
{
}

ImageFilter::~ImageFilter()
{
}

//**************************************************************************
// ImageThreshold
//**************************************************************************

std::map<std::string, cv::ThresholdTypes> mapThreshold = {
    {"Binary", cv::THRESH_BINARY},
    {"Bynary inverted", cv::THRESH_BINARY_INV},
    {"Truncate", cv::THRESH_TRUNC},
    {"Zero", cv::THRESH_TOZERO},
    {"Zero inverted", cv::THRESH_TOZERO_INV},
    {"Masked", cv::THRESH_MASK},
    {"Otsu", cv::THRESH_OTSU},
    {"Triangle", cv::THRESH_TRIANGLE}
}


const std::vector<std::string> ImageThreshold::ThresholdEnum = 
    mapEnum<cv::ThresholdTypes>(mapThreshold);

PROPERTY_SOURCE(Image::ImageThreshold, App::ImageObject)

ImageThreshold::ImageThreshold()
{
    ADD_PROPERTY_TYPE(MaximumValue, (0), "ImageThreshold", App::Prop_None,
        "Maximum value for binary thresholding types");
    ADD_PROPERTY_TYPE(Threshold, (0), "ImageThreshold", App::Prop_None,
        "Threshold value");
    ADD_PROPERTY_TYPE(Type, (0), "ImageThreshold", App::Prop_None,
        "Threshold type");
}

ImageThreshold::~ImageThreshold()
{
}

double ImageThreshold::setThreshold(const double& thresh, const double& maxVal, const int& type)
{
    double threshComp = cv::threshold(baseMat, mat, thresh, maxVal, type);
    return threshComp;
}

App::DocumentObjectExecReturn* ImageThreshold::execute()
{
    if (baseMat.empty())
        return nullptr;
    double threshComp = setThreshold();

}
ImageThreshold::
