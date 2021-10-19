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

#include "ImageContour.h"

#include "opencv2/imgproc.hpp"
#include <Base/Console.h>
using namespace Image;

std::map<std::string, cv::ContourApproximationModes> mapMethod = {
{"None", cv::CHAIN_APPROX_NONE},
{"Simple", cv::CHAIN_APPROX_SIMPLE},
{"Teh-Chin L1", cv::CHAIN_APPROX_TC89_L1},
{"Teh-Chin KCOS", cv::CHAIN_APPROX_TC89_KCOS}
};

const char* ImageContour::MethodEnum[] = {
    "None",
    "Simple",
    "Teh-Chin L1",
    "Teh-Chin KCOS",
    NULL
};

std::map<std::string, cv::RetrievalModes> mapMode = {
{"External", cv::RETR_EXTERNAL},
{"List", cv::RETR_LIST},
{"Two level", cv::RETR_CCOMP},
{"Tree", cv::RETR_TREE},
{"Flood", cv::RETR_FLOODFILL},
};

const char* ImageContour::ModeEnum[] = {
    "External",
    "Flood",
    "List",
    "Tree",
    "Two level",
    NULL
};

std::map<std::string, cv::LineTypes> mapLineStyle = {
    {"Filled", cv::FILLED},
    {"4-Connected", cv::LINE_4},
    {"8-Connected", cv::LINE_8},
    {"Antialiased", cv::LINE_AA},
};

const char* ImageContour::LineStyleEnum[] = {
    "Filled",
    "4-Connected",
    "8-Connected",
    "Antialiased",
    NULL
};


PROPERTY_SOURCE(Image::ImageContour, Image::ImageFilter)

ImageContour::ImageContour()
{
    ADD_PROPERTY_TYPE(Color, (0, 0, 0), "ImageContour", App::Prop_None,
        "Contour line color");
    ADD_PROPERTY_TYPE(Index, (-1), "ImageContour", App::Prop_None,
        "Contour index");
    ADD_PROPERTY_TYPE(Level, (0), "ImageContour", App::Prop_None,
        "Contour level");
    LineStyle.setEnums(LineStyleEnum);
    ADD_PROPERTY_TYPE(LineStyle, (0L), "ImageContour", App::Prop_None,
        "Contour line style");
    Method.setEnums(MethodEnum);
    ADD_PROPERTY_TYPE(Method, (0L), "ImageContour", App::Prop_None,
        "Contour approximation method");
    Mode.setEnums(ModeEnum);
    ADD_PROPERTY_TYPE(Mode, (0L), "ImageContour", App::Prop_None,
        "Contour retrieval mode");
    ADD_PROPERTY_TYPE(Thickness, (1), "ImageContour", App::Prop_None,
        "Contour line thickness");
}

ImageContour::~ImageContour()
{
}

App::DocumentObjectExecReturn* ImageContour::execute()
{
    if (isEmpty())
        return nullptr;

    App::DocumentObjectExecReturn* ret = ImageObject::execute();
    return ret;
}

void ImageContour::onChanged(const App::Property* prop)
{
    if (!linkIsEmpty(&SourceImage) && !linkIsEmpty(&SupportImage)) {
        if (prop == &Method || prop == &Mode) {
            const char* method = Method.getValueAsString();
            const char*  mode = Mode.getValueAsString();
            int index = Index.getValue();
            App::Color color = Color.getValue();
            const char*  line = LineStyle.getValueAsString();
            int thickness = Thickness.getValue();
            int level = Level.getValue();

            findContours(mode, method);
            Base::Console().Message("largo cont:%i\n", _contours.size());
            drawContours(index, color, thickness, line, level);
        }
        else if (prop == &Index || prop == &Color || prop == &LineStyle || prop == &Thickness || prop == &Level) {
            int index = Index.getValue();
            App::Color color = Color.getValue();
            const char*  line = LineStyle.getValueAsString();
            int thickness = Thickness.getValue();
            int level = Level.getValue();
            drawContours(index, color, thickness, line, level);
        }
    }

    ImageObject::onChanged(prop);
}

void ImageContour::findContours(const char* mode, const char* method)
{
    int cvMode = mapMode[mode];
    int cvMethod = mapMethod[method];
    cv::Mat tmpSrc = getLinkMat(&SourceImage);

    cv::findContours(tmpSrc, _contours, _hierarchy, cvMode, cvMethod, _point);
}

void ImageContour::drawContours(const int& index, const App::Color& color,
    const int& thickness, const char* lineType, const int& level)
{
    cv::Scalar cvColor(int(color.b*255), int(color.g*255), int(color.r*255));
    int cvLineType = mapLineStyle[lineType];
    cv::Mat tmpMat = getLinkMat(&SupportImage).clone();

    cv::drawContours(tmpMat, _contours, index, cvColor, thickness, cvLineType, _hierarchy, level, _point);

    MatImage.setValue(tmpMat);
}
