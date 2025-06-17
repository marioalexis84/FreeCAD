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

#ifndef FEM_FEMWORKEREXTENSION_H
#define FEM_FEMWORKEREXTENSION_H

#include <App/ExtensionContainer.h>
#include <App/ExtensionPython.h>
#include <App/PropertyFile.h>

#include <Mod/Fem/FemGlobal.h>


namespace Fem
{
class FemWorkerExtensionPy;

class FemExport FemWorkerExtension: public App::Extension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemWorkerExtension);

public:
    /// Constructor
    FemWorkerExtension();
    ~FemWorkerExtension() override = default;

    PyObject* getExtensionPyObject() override;

    virtual void computeWorker();

    /// Properties
    App::PropertyPath WorkingDirectory;
    App::PropertyPythonObject WorkerTool;
};


template<typename ExtensionT>
class FemWorkerExtensionPythonT: public ExtensionT
{

public:
    FemWorkerExtensionPythonT() = default;
    ~FemWorkerExtensionPythonT() override = default;

    // override the extension functions to make them available in python
    void computeWorker() override
    {
        EXTENSION_PROXY_NOARG(computeWorker);
    };
};

using FemWorkerExtensionPython =
    App::ExtensionPythonT<FemWorkerExtensionPythonT<FemWorkerExtension>>;

}  // namespace Fem


#endif  // FEM_FEMWORKEREXTENSION_H
