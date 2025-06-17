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

#include "FemWorkerExtensionPy.h"


using namespace Fem;

EXTENSION_PROPERTY_SOURCE(Fem::FemWorkerExtension, App::Extension)

FemWorkerExtension::FemWorkerExtension()
{
    initExtensionType(FemWorkerExtension::getExtensionClassTypeId());

    EXTENSION_ADD_PROPERTY_TYPE(
        WorkingDirectory,
        (""),
        "Worker",
        App::PropertyType(App::Prop_Transient | App::Prop_Hidden | App::Prop_Output),
        "Working directory");
    EXTENSION_ADD_PROPERTY_TYPE(
        WorkerTool,
        (Py::Object()),
        "Worker",
        App::PropertyType(App::Prop_Transient | App::Prop_Hidden | App::Prop_Output),
        "Worker tool");
}

PyObject* FemWorkerExtension::getExtensionPyObject()
{
    if (ExtensionPythonObject.is(Py::_None())) {
        // ref counter is set to 1
        auto extPy = new FemWorkerExtensionPy(this);
        ExtensionPythonObject = Py::Object(extPy, true);
    }
    return Py::new_reference_to(ExtensionPythonObject);
}

void FemWorkerExtension::computeWorker()
{}


namespace App
{
EXTENSION_PROPERTY_SOURCE_TEMPLATE(Fem::FemWorkerExtensionPython, Fem::FemWorkerExtension)

template class FemExport ExtensionPythonT<Fem::FemWorkerExtensionPythonT<Fem::FemWorkerExtension>>;
}  // namespace App
