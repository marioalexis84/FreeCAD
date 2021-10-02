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

#include "ImagePipeline.h"

using namespace Image;

PROPERTY_SOURCE(Image::ImagePipeline, App::DocumentObjectGroup)

ImagePipeline::ImagePipeline()
{
//    ADD_PROPERTY_TYPE(File, (0), "ImagePipeline", App::Prop_ReadOnly, "File of the base image");
//    ADD_PROPERTY_TYPE(Group, (0), "ImagePipeline", App::Prop_None, "Processed images");
    ADD_PROPERTY_TYPE(BaseImage, (0), "ImagePipeline", App::Prop_None, "Base image");
    ADD_PROPERTY_TYPE(Tip, (0), "ImagePipeline", App::Prop_None, "Tip image");
}

ImagePipeline::~ImagePipeline()
{
}

//void ImagePipeline::setBaseImage(const char* fileName)
//{
//   File.setValue(fileName);
//}
