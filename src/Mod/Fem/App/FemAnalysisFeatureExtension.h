// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Mario Passaglia <mpassaglia[at]cbc.uba.ar>         *
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

#ifndef FEM_FEMANALYSISFEATUREEXTENSION_H
#define FEM_FEMANALYSISFEATUREEXTENSION_H

#include <App/ExtensionPython.h>
#include <App/DocumentObjectExtension.h>
#include <App/PropertyLinks.h>
#include <Mod/Fem/FemGlobal.h>


namespace Fem
{

class FemExport FemAnalysisFeatureExtension: public App::DocumentObjectExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(FemAnalysisFeatureExtension);

public:
    FemAnalysisFeatureExtension();
    ~FemAnalysisFeatureExtension() override;

    App::PropertyLinkHidden Analysis;
};

using FemAnalysisFeatureExtensionPython = App::ExtensionPythonT<FemAnalysisFeatureExtension>;

}  // namespace Fem


#endif  // FEM_FEMANALYSISFEATUREEXTENSION_H
