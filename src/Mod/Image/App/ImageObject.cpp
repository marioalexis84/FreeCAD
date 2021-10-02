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

#include <Base/FileInfo.h>
#include <Base/Exception.h>
#include "ImageObject.h"

#include <Mod/Image/App/ImageObjectPy.h>

#include "opencv2/imgcodecs.hpp"

using namespace Image;

PROPERTY_SOURCE(Image::ImageObject, App::DocumentObject)

ImageObject::ImageObject()
{
    ADD_PROPERTY_TYPE(BaseImage, (0), "ImageObject", App::Prop_None, "Base image");
}

ImageObject::~ImageObject()
{
}

PyObject* ImageObject::getPyObject(void)
{
    if (PythonObject.is(Py::_None())) {
        // ref counter is set to 1
        PythonObject = Py::Object(new ImageObjectPy(this),true);
    }
    return Py::new_reference_to(PythonObject);
}

int ImageObject::getCols() const
{
    return mat.rows;
}

int ImageObject::getDepth() const
{
    return mat.depth();
}

int ImageObject::getDims() const
{
    return mat.dims;
}

int ImageObject::getRows() const
{
    return mat.cols;
}

int ImageObject::getChannels() const
{
    return mat.channels();
}

size_t ImageObject::getStep(const int& i) const
{
    if (isEmpty())
        throw Base::RuntimeError("The array has no elements");
    if (i >= mat.dims || i < 0)
        throw Base::IndexError("Value must be in the range (0, dims-1)");

    return mat.step[i];
}

bool ImageObject::isContinuous() const
{
    return mat.isContinuous();
}

bool ImageObject::isEmpty() const
{
    return mat.empty();
}

void ImageObject::setBaseMat(const cv::Mat& input)
{
    baseMat = input;
}

void ImageObject::setBaseMat(ImageObject* input)
{
    input->getMat(baseMat);
}

void ImageObject::getBaseMat(cv::Mat& output) const
{
    output = baseMat;
}

void ImageObject::getMat(cv::Mat& output) const
{
    output = mat;
}

void ImageObject::checkBaseMat() const
{
    if (baseMat.empty())
        throw Base::ValueError("Base array is empty");
}

void ImageObject::read(const char* fileName)
{
    Base::FileInfo file(fileName);
    if (!file.isReadable())
        throw Base::FileException("File to load not existing or not readable", fileName);

    baseMat = cv::imread(file.filePath().c_str());
    if(baseMat.empty())
        throw Base::FileException("Error in reading file");
}

void ImageObject::write(const char* fileName) const
{
    Base::FileInfo file(fileName);
    bool result = false;
    try {
        result = cv::imwrite(file.filePath().c_str(), mat);
    }
    catch (const cv::Exception& ex) {
        throw Base::FileException("Can't write file");
    }
    if (!result)       
        throw Base::FileException("Can't write file");
}
