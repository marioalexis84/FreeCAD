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

#include <QString>

#include <Mod/Image/App/ImageObject.h>
#include "ImageView.h"
#include "ViewProviderImageObject.h"

using namespace ImageGui;

static std::map<int, int> mapFormat = {
    {CV_8U, IB_CF_GREY8},
    {CV_16U, IB_CF_GREY16},
    //IB_CF_GREY32},
    //cv::CV_8UC3, IB_CF_RGB24}, 
    //cv::CV_16UC3, IB_CF_RGB48},
    {CV_8UC3, IB_CF_BGR24},
    {CV_16UC3, IB_CF_BGR48}, 
    //cv::CV_8UC4, IB_CF_RGBA32},
    //cv::CV_16UC4, IB_CF_RGBA64},
    {CV_8UC4, IB_CF_BGRA32},
    {CV_16UC4, IB_CF_BGRA64}
};

PROPERTY_SOURCE(ImageGui::ViewProviderImageObject, Gui::ViewProviderDocumentObject)

ViewProviderImageObject::ViewProviderImageObject()
{
    iView = new ImageView(Gui::getMainWindow());
}

ViewProviderImageObject::~ViewProviderImageObject()
{
}

void ViewProviderImageObject::attach(App::DocumentObject* pcObj)
{
    ViewProviderDocumentObject::attach(pcObj);
    if (!pcObj->getTypeId().isDerivedFrom(Image::ImageObject::getClassTypeId()))
        throw Base::TypeError("tipo erroneo");
    Image::ImageObject* pcImg = static_cast<Image::ImageObject*>(pcObj);

    QString str = QString::fromUtf8(pcImg->Label.getValue());
    cv::Mat mat;
    pcImg->getMat(mat);
    iView->resize(400, 300);
    iView->setWindowTitle(str);
    Gui::getMainWindow()->addWindow(iView);
//    if (!pcImg->isEmpty())
//        iView->pointImageTo((void*)mat.data, mat.cols, mat.rows, IB_CF_BGR24, 0, true);
}

void ViewProviderImageObject::updateData(const App::Property* prop)
{
    Image::ImageObject* pcImg = static_cast<Image::ImageObject*>(getObject());
        cv::Mat mat;
        pcImg->getMat(mat);

    if (prop == &pcImg->MatImage && !pcImg->isEmpty()) {
        iView->pointImageTo((void*)mat.data, mat.cols, mat.rows, mapFormat[mat.type()], 0, false);
    }
    else if (prop == &pcImg->Label) {
        QString str = QString::fromUtf8(pcImg->Label.getValue());
        iView->setWindowTitle(str);
    }
}
