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
#include <Base/Console.h>
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

std::map<std::string, int> mapColorCodes = {
    {"Unchanged", -1},
    {"RGB to BGR", cv::COLOR_RGB2BGR},
    {"RGB to RGBA", cv::COLOR_RGB2RGBA},
    {"RGB to BGRA", cv::COLOR_RGB2BGRA},
    {"RGB to GRAY", cv::COLOR_RGB2GRAY},
    {"RGBA to RGB", cv::COLOR_RGBA2RGB},
    {"RGBA to BGR", cv::COLOR_RGBA2BGR},
    {"RGBA to BGRA", cv::COLOR_RGBA2BGRA},
    {"RGBA to GRAY", cv::COLOR_RGBA2GRAY},
    {"BGR to GRAY", cv::COLOR_BGR2GRAY},
    {"BGR to RGB", cv::COLOR_BGR2RGB},
    {"BGR to RGBA", cv::COLOR_BGR2RGBA}, 
    {"BGR to BGRA", cv::COLOR_BGR2BGRA},
    {"BGRA to GRAY", cv::COLOR_BGRA2GRAY},
    {"BGRA to BGR", cv::COLOR_BGRA2BGR},
    {"BGRA to RGB", cv::COLOR_BGRA2RGB}, 
    {"BGRA to RGBA", cv::COLOR_BGRA2RGBA},
};


const char* ImageColor::ColorCodeEnum[] = {
    "Unchanged",
    "BGR to BGRA",
    "BGR to GRAY",
    "BGR to RGB", 
    "BGR to RGBA",
    "BGRA to BGR",
    "BGRA to GRAY",
    "BGRA to RGB",
    "BGRA to RGBA",
    "RGB to BGR", 
    "RGB to BGRA",
    "RGB to GRAY",
    "RGB to RGBA",
    "RGBA to BGR",
    "RGBA to BGRA",
    "RGBA to GRAY",
    "RGBA to RGB",
    NULL
};


PROPERTY_SOURCE(Image::ImageColor, Image::ImageObjectLinked)

ImageColor::ImageColor()
{
    ColorCode.setEnums(ColorCodeEnum);
    ADD_PROPERTY_TYPE(ColorCode, ("Unchanged"), "ImageColor", App::Prop_None, "Color conversion code");
    ADD_PROPERTY_TYPE(File, (0), "ImageColor", App::Prop_None, "File included");
}

ImageColor::~ImageColor()
{
}

short ImageColor::mustExecute() const
{
    if (ColorCode.isTouched())
        return 1;

    return ImageObject::mustExecute();
}

App::DocumentObjectExecReturn* ImageColor::execute()
{
    App::DocumentObjectExecReturn* ret = ImageObjectLinked::execute();
    return ret;
}

void ImageColor::onChanged(const App::Property* prop)
{
    if (!linkIsEmpty(&SourceImage)) {
        if (prop == &ColorCode) {
            const char* code = ColorCode.getValueAsString();
            setColor(code);
        }
    }

    ImageObjectLinked::onChanged(prop);
}

PyObject* ImageColor::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new ImageColorPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}

void ImageColor::setColor(const char* code, int channels)
{
    int cvCode =  mapColorCodes[code];
    cv::Mat tmpSrc = getLinkMat(&SourceImage);

    if (cvCode == -1) {
        MatImage.setValueClone(tmpSrc);
        return;
    }

    try {
    cv::Mat tmpMat;
    cv::cvtColor(tmpSrc, tmpMat, cvCode, channels);
    MatImage.setValue(tmpMat);
    }
    catch (const cv::Exception& ex) {
        throw Base::RuntimeError("Fail to set color conversion code");
    }
}
