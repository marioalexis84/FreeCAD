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


#ifndef IMAGE_PROPERTYIMAGE_H
#define IMAGE_PROPERTYIMAGE_H

#include <App/Property.h>

#include "opencv2/core.hpp"

namespace Image
{

class ImageExport PropertyImage : public App::Property
{
    TYPESYSTEM_HEADER();

public:
    PropertyImage();
    ~PropertyImage();

    /** @name Getter/setter */
    //@{
    /// set the dataset
    void setValue(const cv::Mat& mat);
    /// get the Mat image
    const cv::Mat& getValue() const; //chequear esto con resize por ejemplo
    /// set the dataset with cloned Mat
    void setValueClone(const cv::Mat& mat);
    /// get Mat image cloned
    cv::Mat getValueClone() const;
    //@}

    /** @name Python interface */
    //@{
    PyObject* getPyObject();
    void setPyObject(PyObject* value);
    //@}

    /** @name Save/restore */
    //@{
    void Save (Base::Writer& writer) const;
    void Restore(Base::XMLReader& reader);

    void SaveDocFile (Base::Writer& writer) const;
    void RestoreDocFile(Base::Reader& reader);

    App::Property* Copy() const;
    void Paste(const App::Property& from);
    unsigned int getMemSize () const;
    //@}

    /// Get valid paths for this property; used by auto completer
    virtual void getPaths(std::vector<App::ObjectIdentifier>& paths) const;

protected:
//    void createDataObjectByExternalType(vtkSmartPointer<vtkDataObject> ex);
    cv::Mat mat;
};

} //namespace Image


#endif // IMAGE_PROPERTYIMAGE_H
