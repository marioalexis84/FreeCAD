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

#include /*"Mod/Image/App*/"ImageObject.h"

// inclusion of the generated files (generated out of ImageObjectPy.xml)
#include "Mod/Image/App/ImageObjectPy.h"
#include "Mod/Image/App/ImageObjectPy.cpp"

#include <Base/Console.h>
using namespace Image;

// returns a string which represents the object e.g. when printed in python
std::string ImageObjectPy::representation() const
{
    return std::string("<ImageObject object>");
}

Py::Int ImageObjectPy::getCols() const
{
    int cols = getImageObjectPtr()->getCols();

    return Py::Int(cols);
}

Py::Int ImageObjectPy::getRows() const
{
    int rows = getImageObjectPtr()->getRows();

    return Py::Int(rows);
}

Py::Int ImageObjectPy::getChannels() const
{
    int channels = getImageObjectPtr()->getChannels();

    return Py::Int(channels);
}

Py::Int ImageObjectPy::getDepth() const
{
    int depth = getImageObjectPtr()->getDepth();

    return Py::Int(depth);
}

Py::Int ImageObjectPy::getDims() const
{
    int dims = getImageObjectPtr()->getDims();

    return Py::Int(dims);
}

PyObject* ImageObjectPy::step(PyObject *args)
{
    int i;
    if (!PyArg_ParseTuple(args, "i", &i))
        return nullptr;

    const size_t step = getImageObjectPtr()->getStep(i);

    return PyLong_FromSize_t(step);
}

Py::Boolean ImageObjectPy::getIsContinuous() const
{
    bool continuous = getImageObjectPtr()->isContinuous();

    return Py::Boolean(continuous);
}

Py::Boolean ImageObjectPy::getIsEmpty() const
{
    bool empty = getImageObjectPtr()->isEmpty();

    return Py::Boolean(empty);
}

PyObject* ImageObjectPy::read(PyObject *args)
{
    char *fileName;
    if (!PyArg_ParseTuple(args, "s", &fileName))
        return nullptr;

    getImageObjectPtr()->read(fileName);
//    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    Py_Return;
}

PyObject* ImageObjectPy::write(PyObject *args)
{
    char *fileName;
    if (!PyArg_ParseTuple(args, "s", &fileName))
        return nullptr;

    getImageObjectPtr()->write(fileName);
//    PyErr_SetString(PyExc_NotImplementedError, "Not yet implemented");
    Py_Return;
}





PyObject *ImageObjectPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ImageObjectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}


