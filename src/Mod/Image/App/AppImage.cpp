/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <Python.h>
#endif

#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>

#include <Base/Console.h>
#include <Base/PyObjectBase.h>
#include "ImageColor.h"
#include "ImageFilter.h"
#include "ImagePipeline.h"
#include "ImagePlane.h"
#include "PropertyImage.h"
#include "ImageThreshold.h"

namespace Image {
class Module : public Py::ExtensionModule<Module>
{
public:
    Module() : Py::ExtensionModule<Module>("Image")
    {
        initialize("This module is the Image module."); // register with Python
    }

    virtual ~Module() {}

private:
};

PyObject* initModule()
{
    return (new Module)->module().ptr();
}

} // namespace Image

/* Python entry */
PyMOD_INIT_FUNC(Image)
{
    PyObject* mod = Image::initModule();
    Base::Console().Log("Loading Image module... done\n");

    Image::PropertyImage::init();
    Image::ImageObject::init();
    Image::ImageObjectLinked::init();
    Image::ImageColor::init();
    Image::ImagePipeline::init();
    Image::ImageFilter::init();
    Image::ImageThreshold::init();
    Image::ImagePlane::init();

    PyMOD_Return(mod);
}
