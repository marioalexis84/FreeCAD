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

#include "Mod/Image/App/ImageColor.h"

// inclusion of the generated files (generated out of ImageColorPy.xml)
#include "Mod/Image/App/ImageColorPy.h"
#include "Mod/Image/App/ImageColorPy.cpp"

using namespace Image;

// returns a string which represents the object e.g. when printed in python
std::string ImageColorPy::representation() const
{
    return std::string("<ImageColor object>");
}



PyObject* ImageColorPy::setColor(PyObject *args)
{
    const char* code;
    int channels = 0;
    if(!PyArg_ParseTuple(args, "s|i", &code, &channels))
        return nullptr;

    getImageColorPtr()->setColor(code, channels);
    Py_Return;
}





PyObject *ImageColorPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ImageColorPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


