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
#include "ImageObject.h"

using namespace Image;

PROPERTY_SOURCE(Image::ImagePipeline, App::DocumentObjectGroup)

ImagePipeline::ImagePipeline()
{
    ADD_PROPERTY_TYPE(BaseImage, (0), "ImagePipeline", App::Prop_None, "Base image");
    ADD_PROPERTY_TYPE(Tip, (0), "ImagePipeline", App::Prop_None, "Tip image");
}

ImagePipeline::~ImagePipeline()
{
}

bool ImagePipeline::allowObject(App::DocumentObject* obj)
{
    if (!obj)
        return false;
    else
        return obj->getTypeId().isDerivedFrom(ImageObjectLinked::getClassTypeId());
}

std::vector<App::DocumentObject*> ImagePipeline::addObject(App::DocumentObject* doc)
{
    App::DocumentObject* base = BaseImage.getValue();
    if (!base)
        throw Base::ValueError("ImagePipeline: Set base image before add objects");

    if (!allowObject(doc))
        throw Base::ValueError("ImagePipeline: Object not allowed");

    std::vector<App::DocumentObject*> group = Group.getValues();
    ImageObjectLinked* linkImg = static_cast<ImageObjectLinked*>(doc);
    ImageObject* srcImg = nullptr;

    if (group.empty()) {
        srcImg = static_cast<ImageObject*>(base);
    }
    else {
        srcImg = static_cast<ImageObject*>(group.back());
    }

    (linkImg->SourceImage).setValue(srcImg);
    (linkImg->Pipeline).setValue(this);
    group.push_back(linkImg);
    Group.setValues(group);
    Tip.setValue(linkImg);

    return std::vector<App::DocumentObject*>(1, doc);
}

std::vector<App::DocumentObject*> ImagePipeline::addObjects(std::vector<App::DocumentObject*> docs)
{
    for (auto obj: docs) {
        addObject(obj);
    }

    return docs;
}

//void ImagePipeline::onChanged(const App::Property* prop)
//{
//    if (prop == &BaseImage) {
//        App::DocumentObject* base = BaseImage.getValue();
//        if (!allowObject(base)) {
//           return;
//        }        
//    }
//    else if (prop == &Group) {
//        std::vector<App::DocumentObject*> group = Group.getValue();
//        App::DocumentObject* base = BaseImage.getValue();
//        if (!group.empty() && base) {
//            Tip.setValue(group.back());
//            ImageObjectLinked* tip = static_cast<ImageObjectLinked*>(group.front())
//            
//        }
//    }
//    DocumentObjectGroup.onChanged(prop);
//}
