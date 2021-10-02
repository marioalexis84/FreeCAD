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

#ifndef Image_ImageFilter_H
#define Image_ImageFilter_H

#include <App/PropertyStandard.h>

#include "ImageObject.h"

namespace Image
{

class ImageExport ImageFilter : public ImageObject
{
    PROPERTY_HEADER(Image::ImageFilter);

public:
    ImageFilter();
    ~ImageFilter();

};

class ImageExport ImageThreshold : public ImageFilter
{
    PROPERTY_HEADER(Image::ImageThreshold);

public:
    ImageThreshold();
    ~ImageThreshold();

    App::PropertyFloat Threshold;
    App::PropertyFloat MaximumValue;
    App::PropertyEnumeration Type;

    App::DocumentObjectExecReturn* execute();
    PyObject* getPyObject();

    virtual const char* getViewProviderName(void) const
    {
        return "Gui::ViewProviderDocumentObject";
    }

protected:
    static const std::vector<std::string> ThresholdEnum;
    double setThreshold(const double& thresh, const double& maxVal, cons int& type);
};

} // namespace Image

#endif // Image_ImageFilter_H
