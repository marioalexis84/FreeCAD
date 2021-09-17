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

using namespace Image;

const char* ImageColor::ColorCodeEnum[] = {"rgb2rgba", "rgba2rgb", "rgb2bgra" ,"rgba2bgr", "rgb2bgr", "rgba2bgra","bgr2gray", "rgb2gray", NULL};
PROPERTY_SOURCE(Image::ImageColor, App::DocumentObject)

ImageColor::ImageColor()
{
    ADD_PROPERTY_TYPE(ColorCode, (6L), "ImageColor", App::Prop_None, "Color conversion code");
    ColorCode.setEnums(ColorCodeEnum);
}

ImageColor::~ImageColor()
{
}

App::DocumentObjectExecReturn* ImageColor::execute()
{
    long code = ColorCode.getValue();
    setColor(code);
    return nullptr;
}

void ImageColor::setColor(int code, int channels)
{
    cv::cvtColor(this->baseMat, this->mat, code, channels);
}
