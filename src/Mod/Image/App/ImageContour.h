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

#ifndef Image_ImageContour_H
#define Image_ImageContour_H

#include <App/PropertyStandard.h>

#include "ImageFilter.h"

namespace Image
{

class ImageExport ImageContour : public ImageFilter
{
    PROPERTY_HEADER(Image::ImageContour);

public:
    ImageContour();
    ~ImageContour();

    App::PropertyEnumeration Mode;
    App::PropertyEnumeration Method;
    App::PropertyColor Color;
    App::PropertyInteger Index;
    App::PropertyEnumeration LineStyle;
    App::PropertyInteger Thickness;
    App::PropertyInteger Level;

    App::DocumentObjectExecReturn* execute();
    void onChanged(const App::Property* prop);
//    PyObject* getPyObject();

    virtual const char* getViewProviderName(void) const
    {
        return "ImageGui::ViewProviderImageObject";
    }

protected:
    static const char* ModeEnum[];
    static const char* MethodEnum[];
    static const char* LineStyleEnum[];
    void findContours(const char* mode, const char* method);
    void drawContours(const int& index, const App::Color& color,
        const int& thickness, const char* lineType, const int& level);

private:
    std::vector<std::vector<cv::Point>> _contours;
    std::vector<cv::Vec4i> _hierarchy;
    cv::Point _point = cv::Point();
};

} // namespace Image

#endif // Image_ImageContour_H
