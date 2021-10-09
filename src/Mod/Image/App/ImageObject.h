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

#ifndef Image_ImageObject_H
#define Image_ImageObject_H

#include <App/DocumentObject.h>

#include "PropertyImage.h"

#include "opencv2/core.hpp"

namespace Image
{

class ImageExport ImageObject: public App::DocumentObject
{
    PROPERTY_HEADER(Image::ImageObject);

public:
    ImageObject();
    ~ImageObject();

    Image::PropertyImage MatImage;

    App::DocumentObjectExecReturn* execute();
    PyObject* getPyObject();

    virtual const char* getViewProviderName(void) const
    {
        return "ImageGui::ViewProviderImageObject";
    }

    int getCols() const;
    int getDims() const;
    int getDepth() const;
    int getRows() const;
    int getChannels() const;
    size_t getStep(const int& i) const;

    bool isContinuous() const;
    bool isEmpty() const;

    void getMat(cv::Mat& output) const;  

    void read(const char* fileName);
    void write(const char* fileName) const;

protected:

private:
    const cv::Mat* _mat; // = MatImage.getValue();
};


class ImageExport ImageObjectLinked: public ImageObject
{
    PROPERTY_HEADER(Image::ImageObjectLinked);

public:
    ImageObjectLinked();
    ~ImageObjectLinked();

    App::PropertyLink BaseImage;
    App::PropertyLink Pipeline;

    void getBaseMat(cv::Mat& output) const;

    ImageObject* checkBase() const;
    void setBaseMat(ImageObject* input);

    
    bool baseIsEmpty() const;
};

} // namespace Image

#endif // Image_ImageObject_H
