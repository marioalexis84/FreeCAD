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

#include "ImageColor.h"
#include "Mod/Image/App/ImageColorPy.h"

#include "opencv2/highgui.hpp"

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

std::map<std::string, cv::ColorConversionCodes> mapColorCodes = {
   {"RGB to RGBA", cv::COLOR_RGB2RGBA},
   {"RGBA to RGB", cv::COLOR_RGBA2RGB},
   {"RGB to BGRA", cv::COLOR_RGB2BGRA},
   {"RGBA to BGR", cv::COLOR_RGBA2BGR},
   {"RGB to BGR", cv::COLOR_RGB2BGR},
   {"RGBA to BGRA", cv::COLOR_RGBA2BGRA},
   {"BGR to GRAY", cv::COLOR_BGR2GRAY},
   {"RGB to GRAY", cv::COLOR_RGB2GRAY},
   {"BGRA to GRAY", cv::COLOR_BGRA2GRAY},
   {"RGBA to GRAY", cv::COLOR_RGBA2GRAY}  ,
 //  cv::COLOR_BGR2BGRA = 0,
//   = COLOR_BGR2BGRA,
//  cv::COLOR_BGRA2BGR = 1,
// = COLOR_BGRA2BGR,
//  cv::COLOR_BGR2RGBA = 2,
//   = COLOR_BGR2RGBA,
//   = 3,
//  cv::COLOR_BGRA2RGB = COLOR_RGBA2BGR,
//  cv::COLOR_BGR2RGB = 4,
//   = COLOR_BGR2RGB,
//  cv::COLOR_BGRA2RGBA = 5,
//  = COLOR_BGRA2RGBA,
//  = 6,
//   = 7,
//  cv::COLOR_GRAY2BGR = 8,
//  cv::COLOR_GRAY2RGB = COLOR_GRAY2BGR,
//  cv::COLOR_GRAY2BGRA = 9,
//  cv::COLOR_GRAY2RGBA = COLOR_GRAY2BGRA,
//  cv::COLOR_BGRA2GRAY = 10, 

};

const std::vector<std::string> ImageColor::ColorCodeEnum = mapEnum(mapColorCodes);

PROPERTY_SOURCE(Image::ImageColor, Image::ImageObject)

ImageColor::ImageColor()
{
    ADD_PROPERTY_TYPE(ColorCode, (6L), "ImageColor", App::Prop_None, "Color conversion code");
    ColorCode.setEnums(ColorCodeEnum);
    ADD_PROPERTY_TYPE(File, (0), "ImageColor", App::Prop_None, "File included");
}

ImageColor::~ImageColor()
{
}

App::DocumentObjectExecReturn* ImageColor::execute()
{
//    cv::Mat arch = cv::imread(File.getValue());
//    setBaseMat(arch);
    int code = ColorCode.getValue();
    setColor(code);
//    cv::imshow("sarasa", mat);
//    cv::waitKey(0);
    return nullptr;
}

PyObject* ImageColor::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new ImageColorPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}

void ImageColor::setColor(int code, int channels)
{
    try {
    cv::cvtColor(this->baseMat, this->mat, code, channels);
    }
    catch (const cv::Exception& ex) {
        throw Base::RuntimeError("Fail to set color conversion code");
    }
}
