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
#include <Base/Console.h>
using namespace Image;

//***************************************************************************
// ImageObject
//***************************************************************************

PROPERTY_SOURCE(Image::ImageObject, App::DocumentObject)

ImageObject::ImageObject()
{
    ADD_PROPERTY_TYPE(MatImage, (cv::Mat()), "ImageObject", App::Prop_None, "Mat image");
    ADD_PROPERTY_TYPE(Height, (0), "ImageObject", App::Prop_ReadOnly, "Image height");
    ADD_PROPERTY_TYPE(Width, (0), "ImageObject", App::Prop_ReadOnly, "Image width");
    ADD_PROPERTY_TYPE(Channels, (1), "ImageObject", App::Prop_ReadOnly, "Image channels");
    ADD_PROPERTY_TYPE(Depth, (0), "ImageObject", App::Prop_ReadOnly, "Image element depth");
    _mat = &MatImage.getValue();
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

void ImageObject::onChanged(const App::Property* prop)
{
    if (prop == &MatImage) {
        Channels.setValue(_mat->channels());
        Width.setValue(_mat->cols);
        Height.setValue(_mat->rows);
        Depth.setValue(_mat->depth());
    }

    DocumentObject::onChanged(prop);
}

App::DocumentObjectExecReturn* ImageObject::execute()
{
//    mat = baseMat;
    return DocumentObject::execute();
}

int ImageObject::getCols() const
{
    return _mat->rows;
}

int ImageObject::getDepth() const
{
    return _mat->depth();
}

int ImageObject::getDims() const
{
    return _mat->dims;
}

int ImageObject::getRows() const
{
    return _mat->cols;
}

int ImageObject::getChannels() const
{
    return _mat->channels();
}

size_t ImageObject::getStep(const int& i) const
{
    if (isEmpty())
        throw Base::RuntimeError("The array has no elements");
    if (i >= _mat->dims || i < 0)
        throw Base::IndexError("Value must be in the range (0, dims-1)");

    return _mat->step[i];
}

bool ImageObject::isContinuous() const
{
    return _mat->isContinuous();
}

bool ImageObject::isEmpty() const
{
    return _mat->empty();
}

void ImageObject::getMat(cv::Mat& output) const
{
    output = *_mat;
}

void ImageObject::read(const char* fileName)
{
    Base::FileInfo file(fileName);
    if (!file.isReadable())
        throw Base::FileException("File to load not existing or not readable", fileName);

    cv::Mat img = cv::imread(file.filePath().c_str());
    MatImage.setValue(img);

    if(_mat->empty())
        throw Base::FileException("Error in reading file");
}

void ImageObject::write(const char* fileName) const
{
    Base::FileInfo file(fileName);
    bool result = false;
    try {
        result = cv::imwrite(file.filePath().c_str(), *_mat);
    }
    catch (const cv::Exception& ex) {
        throw Base::FileException("Can't write file");
    }
    if (!result)       
        throw Base::FileException("Can't write file");
}

//***************************************************************************
// ImageObjectLinked
//***************************************************************************

PROPERTY_SOURCE(Image::ImageObjectLinked, Image::ImageObject)

ImageObjectLinked::ImageObjectLinked()
{
    ADD_PROPERTY_TYPE(SourceImage, (nullptr), "Base", App::Prop_None, "Source image");
    ADD_PROPERTY_TYPE(SupportImage, (nullptr), "Base", App::Prop_None, "Support image");
    ADD_PROPERTY_TYPE(Pipeline, (nullptr), "Base", App::Prop_Hidden, "Pipeline group");
}

ImageObjectLinked::~ImageObjectLinked()
{
}

void ImageObjectLinked::onChanged(const App::Property* prop)
{
    if (prop == &SourceImage || prop == &SupportImage) {
       ImageObject::recompute();
    }

    ImageObject::onChanged(prop);
}

const cv::Mat& ImageObjectLinked::getLinkMat(App::PropertyLink* prop) const
{
    ImageObject* link = checkLink(prop);
    if (link) {
        const cv::Mat& output = link->MatImage.getValue();
        return output;
    }
    else {
        throw Base::ValueError("Link must be no empty Image object");
    }
}

void ImageObjectLinked::setSourceMat(ImageObject* input)
{
    SourceImage.setValue(input);
}

ImageObject* ImageObjectLinked::checkLink(App::PropertyLink* prop) const
{
    App::DocumentObject* link = prop->getValue();
    if (link && link->isDerivedFrom(ImageObject::getClassTypeId())) {
        ImageObject* linkImg = static_cast<ImageObject*>(link);
        return linkImg;
    }
    else {
        return nullptr;
    }
}

bool ImageObjectLinked::linkIsEmpty(App::PropertyLink* prop) const
{
    ImageObject* link = checkLink(prop);

    if (link)
        return link->isEmpty();
    else
        return true;
}
