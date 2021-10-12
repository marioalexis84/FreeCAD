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

#ifndef Image_ImagePipeline_H
#define Image_ImagePipeline_H

#include <App/DocumentObjectGroup.h>
#include <App/PropertyStandard.h>
#include <App/PropertyLinks.h>
//#include <App/FeaturePython.h>

namespace Image
{

class ImageExport ImagePipeline : public App::DocumentObjectGroup
{
    PROPERTY_HEADER(Image::ImagePipeline);

public:
    ImagePipeline();
    ~ImagePipeline();

    App::PropertyLinkList Group;
    App::PropertyLink BaseImage;
    App::PropertyLink Tip;

//    void setBaseImage(const char* fileName);
//    PyObject* getPyObject() override;
};

//typedef App::FeaturePythonT<ImagePipeline> ImagePipelinePython;

} // namespace Image

#endif // Image_ImagePipeline_H
