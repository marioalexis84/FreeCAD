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

const char* ImageColor::ColorCodeEnum[] = {"rgb2rgba", "rgba2rgb", "rgb2bgra" ,"rgba2bgr", "rgb2bgr", "rgba2bgra","bgr2gray", "rgb2gray", NULL};

PROPERTY_SOURCE(Image::ImageColor, App::DocumentObject)

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
    long code = ColorCode.getValue();
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
    cv::cvtColor(this->baseMat, this->mat, code, channels);
}
