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
    {"RGB to RGBA", cv::COLOR_RGB2RGBA},
    {"RGBA to RGB", cv::COLOR_RGBA2RGB},
    {"RGB to BGRA", cv::COLOR_RGB2BGRA},
    {"RGBA to BGR", cv::COLOR_RGBA2BGR},
    {"RGB to BGR", cv::COLOR_RGB2BGR},
    {"RGBA to BGRA", cv::COLOR_RGBA2BGRA},
    {"BGR to GRAY", cv::COLOR_BGR2GRAY},
    {"RGB to GRAY", cv::COLOR_RGB2GRAY},
    {"BGR to RGB", cv::COLOR_BGR2RGB},
    {"BGRA to GRAY", cv::COLOR_BGRA2GRAY},
    {"RGBA to GRAY", cv::COLOR_RGBA2GRAY}
};

const std::vector<std::string> ImageColor::ColorCodeEnum = mapEnum(mapColorCodes);

PROPERTY_SOURCE(Image::ImageColor, Image::ImageObjectLinked)

ImageColor::ImageColor()
{
    ColorCode.setEnums(ColorCodeEnum);
    ADD_PROPERTY_TYPE(ColorCode, ("RGB to BGR"), "ImageColor", App::Prop_None, "Color conversion code");
    Base::Console().Message("el color:%s\t%i\n", ColorCode.getValueAsString(), ColorCode.getValue());
    ADD_PROPERTY_TYPE(File, (0), "ImageColor", App::Prop_None, "File included");
}

ImageColor::~ImageColor()
{
}

short ImageColor::mustExecute() const
{
    Base::Console().Message("color_mustExecute\n");
    if (ColorCode.isTouched())
        return 1;

    return ImageObject::mustExecute();
}

App::DocumentObjectExecReturn* ImageColor::execute()
{
//    cv::Mat arch = cv::imread(File.getValue());
//    setBaseMat(arch);
//    const char* code = ColorCode.getValueAsString();
    if (isEmpty())
        return StdReturn;
//    setColor(code);
//    cv::imshow("sarasa", mat);
//    cv::waitKey(0);
    Base::Console().Message("color_execute:%i\t%s\n", getChannels(), ColorCode.getName());
    App::DocumentObjectExecReturn* ret = ImageObject::execute();
    Base::Console().Message("color_execute2:%i\t%s\n", getChannels(), ColorCode.getName());
    return ret;
}

void ImageColor::onChanged(const App::Property* prop)
{
    if (prop == &ColorCode) {
        if (!baseIsEmpty()) {
            const char* code = ColorCode.getValueAsString();
            setColor(code);
        }
    }

    ImageObject::onChanged(prop);
    Base::Console().Message("color_onchanged:%i\t%s\n", getChannels(), prop->getName());
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
    cv::Mat tmpBase;
    getBaseMat(tmpBase);

    if (cvCode == -1) {
        MatImage.setValueClone(tmpBase);
        return;
    }

    try {
    cv::Mat tmpMat;
    cv::cvtColor(tmpBase, tmpMat, cvCode, channels);
    MatImage.setValue(tmpMat);
    Base::Console().Error("tmpCols:%i\tMatImageCols:%i\n", tmpMat.cols, MatImage.getValue().cols);
    }
    catch (const cv::Exception& ex) {
        throw Base::RuntimeError("Fail to set color conversion code");
    }
}
