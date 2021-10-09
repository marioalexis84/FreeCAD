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
#include <Gui/MainWindow.h>
#include <Mod/Image/App/ImageObject.h>
#include "ImageView.h"
#include "ViewProviderImageObject.h"

using namespace ImageGui;

PROPERTY_SOURCE(ImageGui::ViewProviderImageObject, Gui::ViewProviderDocumentObject)

ViewProviderImageObject::ViewProviderImageObject()
{
    Base::Console().Message("object_constr\n");
    iView = new ImageView(Gui::getMainWindow());
}

ViewProviderImageObject::~ViewProviderImageObject()
{
    Base::Console().Message("object_destruct\n");
}

void ViewProviderImageObject::attach(App::DocumentObject* pcObj)
{
    ViewProviderDocumentObject::attach(pcObj);
    Base::Console().Message("object_attach:%s\n", pcObj->getTypeId().getName());
    if (!pcObj->getTypeId().isDerivedFrom(Image::ImageObject::getClassTypeId()))
        throw Base::TypeError("tipo erroneo");
    Image::ImageObject* pcImg = static_cast<Image::ImageObject*>(pcObj);

    cv::Mat mat;
    pcImg->getMat(mat);
    iView->resize(400, 300);
    Gui::getMainWindow()->addWindow(iView);
    if (!pcImg->isEmpty())
        iView->pointImageTo((void*)mat.data, mat.cols, mat.rows, IB_CF_BGR24, 0, true);
}

void ViewProviderImageObject::updateData(const App::Property* prop)
{
    Base::Console().Message("object_update:%s\n", prop->getName());
}
