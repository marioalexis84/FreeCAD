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

#include <Base/FileInfo.h>
#include <Base/Console.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <App/Application.h>
#include <App/DocumentObject.h>

#include "opencv2/imgcodecs.hpp"

#include "PropertyImage.h"


using namespace Image;

TYPESYSTEM_SOURCE(Image::PropertyImage, App::Property)

PropertyImage::PropertyImage()
{
}

PropertyImage::~PropertyImage()
{
}

void PropertyImage::setValue(const cv::Mat& matValue)
{
    aboutToSetValue();
    mat = matValue;
    hasSetValue();
}

const cv::Mat& PropertyImage::getValue() const
{
    return mat;
}

void PropertyImage::setValueClone(const cv::Mat& matValue)
{
    aboutToSetValue();
    mat = matValue.clone();
    hasSetValue();
}

cv::Mat PropertyImage::getValueClone() const
{
    return mat.clone();
}

PyObject *PropertyImage::getPyObject()
{
    //TODO: fetch the OpenCV python object from the data set and return it
//    return new PyObject();
    Py_Return;
}

void PropertyImage::setPyObject(PyObject * /*value*/)
{
}

App::Property* PropertyImage::Copy() const
{
    PropertyImage* prop = new PropertyImage();
    prop->mat = mat.clone();

    return prop;
}


void PropertyImage::Paste(const App::Property& from)
{
    aboutToSetValue();
    mat = dynamic_cast<const PropertyImage&>(from).mat;
    hasSetValue();
}

unsigned int PropertyImage::getMemSize() const
{
    return mat.step[0]*mat.rows;
}

void PropertyImage::getPaths(std::vector<App::ObjectIdentifier> & /*paths*/) const
{
//     paths.push_back(App::ObjectIdentifier(getContainer()) << App::ObjectIdentifier::Component::SimpleComponent(getName())
//                     << App::ObjectIdentifier::Component::SimpleComponent(App::ObjectIdentifier::String("ShapeType")));
//     paths.push_back(App::ObjectIdentifier(getContainer()) << App::ObjectIdentifier::Component::SimpleComponent(getName())
//                     << App::ObjectIdentifier::Component::SimpleComponent(App::ObjectIdentifier::String("Orientation")));
//     paths.push_back(App::ObjectIdentifier(getContainer()) << App::ObjectIdentifier::Component::SimpleComponent(getName())
//                     << App::ObjectIdentifier::Component::SimpleComponent(App::ObjectIdentifier::String("Length")));
//     paths.push_back(App::ObjectIdentifier(getContainer()) << App::ObjectIdentifier::Component::SimpleComponent(getName())
//                     << App::ObjectIdentifier::Component::SimpleComponent(App::ObjectIdentifier::String("Area")));
//     paths.push_back(App::ObjectIdentifier(getContainer()) << App::ObjectIdentifier::Component::SimpleComponent(getName())
//                     << App::ObjectIdentifier::Component::SimpleComponent(App::ObjectIdentifier::String("Volume")));
}

void PropertyImage::Save (Base::Writer &writer) const
{
    std::string extension = "png";
    if(mat.empty())
        return;

    if(!writer.isForceXML()) {
        std::string file = "Image." + extension;
        writer.Stream() << writer.ind() << "<Image file=\""
                        << writer.addFile(file.c_str(), this)
                        << "\"/>" << std::endl;
    }
}

void PropertyImage::Restore(Base::XMLReader &reader)
{
    reader.readElement("Image");
    if(!reader.hasAttribute("file"))
        return;

    std::string file (reader.getAttribute("file") );

    if (!file.empty()) {
        // initiate a file read
        reader.addFile(file.c_str(),this);
    }
}

void PropertyImage::SaveDocFile (Base::Writer &writer) const
{
    // If the cv::Mat is empty we simply store nothing. The file size will be 0 which
    // can be checked when reading in the data.
    if (mat.empty())
        return;

    // create a temporary file and copy the content to the zip stream
    // once the tmp. filename is known use always the same because otherwise
    // we may run into some problems on the Linux platform
    std::stringstream str;
    str << App::Application::getTempFileName() << ".png";
    static Base::FileInfo fi(str.str());

    cv::imwrite(fi.filePath().c_str(), mat);
    Base::ifstream file(fi, std::ios::in | std::ios::binary);
    if (file){
        std::streambuf* buf = file.rdbuf();
        writer.Stream() << buf;
    }

    file.close();
    // remove temp file
    fi.deleteFile();
}

void PropertyImage::RestoreDocFile(Base::Reader &reader)
{
    // create a temporary file and copy the content from the zip stream
    Base::FileInfo fi(App::Application::getTempFileName());

    // read in the ASCII file and write back to the file stream
    Base::ofstream file(fi, std::ios::out | std::ios::binary);
    unsigned long ulSize = 0;
    if (reader) {
        std::streambuf* buf = file.rdbuf();
        reader >> buf;
        file.flush();
        ulSize = buf->pubseekoff(0, std::ios::cur, std::ios::in);
    }
    file.close();

    // Read the data from the temp file
    if (ulSize > 0) {
    aboutToSetValue();
    mat = cv::imread(fi.filePath().c_str());
    Base::Console().Error("mat.rows: %i\n", mat.rows);
    hasSetValue();
    }

    // delete the temp file
    fi.deleteFile();
}
