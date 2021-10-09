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

#include <Base/Exception.h>
#include <Mod/Image/App/ImageColor.h>
#include "ImageView.h"
#include "ViewProviderImageColor.h"

using namespace ImageGui;

PROPERTY_SOURCE(ImageGui::ViewProviderImageColor, ImageGui::ViewProviderImageObject)

ViewProviderImageColor::ViewProviderImageColor()
{
}

ViewProviderImageColor::~ViewProviderImageColor()
{
}

//void ViewProviderImageColor::attach(App::DocumentObject* pcObj)
//{
//    ViewProviderDocumentObject::attach(pcObj);
//    Base::Console().Message("tipo:%s\n", pcObj->getTypeId().getName());
//    if (!pcObj->getTypeId().isDerivedFrom(Image::ImageObject::getClassTypeId()))
//        throw Base::TypeError("tipo erroneo");
//    Image::ImageObject* pcImg = static_cast<Image::ImageObject*>(pcObj);
//
//    cv::Mat mat;
//    pcImg->getMat(mat);
//    ImageView* iView = new ImageView(Gui::getMainWindow());
//    iView->resize(400, 300);
//    Gui::getMainWindow()->addWindow(iView);
//    iView->pointImageTo((void*)mat.data, mat.rows, mat.cols, IB_CF_RGB24, 0, true);
//}

void ViewProviderImageColor::updateData(const App::Property* prop)
{
    Image::ImageColor* pcImg = static_cast<Image::ImageColor*>(pcObject);

    if (prop == &pcImg->ColorCode && !pcImg->baseIsEmpty()) {
//        pcImg->setColor((pcImg->ColorCode).getValueAsString());
        cv::Mat mat;
        pcImg->getMat(mat);
        Base::Console().Error("color_update:%i\t%s\n", pcImg->getChannels(), (pcImg->ColorCode).getValueAsString());
        iView->pointImageTo((void*)mat.data, mat.cols, mat.rows, IB_CF_BGR24, 0, false);
        Base::Console().Message("color_update_2:%s\n", pcObject->getTypeId().getName());
    }
    Base::Console().Message("color_update_3:%s\n", prop->getName());

    ViewProviderImageObject::updateData(prop);
        Base::Console().Error("color_update_4:%i\t%s\n", pcImg->getChannels(), (pcImg->ColorCode).getValueAsString());
}
