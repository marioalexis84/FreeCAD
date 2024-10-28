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

#ifndef FEM_VIEWPROVIDERANALYSISFEATUREEXTENSION_H
#define FEM_VIEWPROVIDERANALYSISFEATUREEXTENSION_H


#include <Gui/ViewProviderExtensionPython.h>
#include <Mod/Fem/FemGlobal.h>


namespace FemGui
{

class FemAnalysis;


class FemGuiExport ViewProviderFemAnalysisFeatureExtension: public Gui::ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemAnalysisFeatureExtension);

public:
    // Constructor
    ViewProviderFemAnalysisFeatureExtension();
    ~ViewProviderFemAnalysisFeatureExtension() override;

    //    std::vector<App::DocumentObject*> extensionClaimChildren() const override;
    void extensionUpdateData(const App::Property* prop) override;
};

using ViewProviderFemAnalysisFeatureExtensionPython =
    Gui::ViewProviderExtensionPythonT<ViewProviderFemAnalysisFeatureExtension>;
// template<typename ExtensionT>
// class FemAnalysisFeatureExtensionPythonT : public ExtensionT
//{
// public:
//
//     FemAnalysisFeatureExtensionPythonT() = default;
//     ~FemAnalysisFeatureExtensionPythonT() override = default;
// };
//
// using FemAnalysisFeatureExtensionPython =
// App::ExtensionPythonT<FemAnalysisFeatureExtensionPythonT<FemAnalysisFeatureExtension>>;

}  // namespace FemGui


#endif  // FEM_VIEWPROVIDERANALYSISFEATUREEXTENSION_H
