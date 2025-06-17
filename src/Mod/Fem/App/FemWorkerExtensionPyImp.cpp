// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2025 Mario Passaglia <mpassaglia[at]cbc.uba.ar>         *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 **************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
#endif

#include <Base/Interpreter.h>

#include "FemWorkerExtensionPy.h"
#include "FemWorkerExtensionPy.cpp"


using namespace Fem;

// returns a string which represent the object e.g. when printed in python
std::string FemWorkerExtensionPy::representation() const
{
    std::stringstream str;
    str << "<FemWorkerExtension object at " << getFemWorkerExtensionPtr() << ">";

    return str.str();
}

PyObject* FemWorkerExtensionPy::computeWorker(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    getFemWorkerExtensionPtr()->computeWorker();

    Py_Return;
}

PyObject* FemWorkerExtensionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int FemWorkerExtensionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
