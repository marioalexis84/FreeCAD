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

#include <Gui/ViewProviderDocumentObject.h>
#include <Mod/Fem/App/FemAnalysisFeatureExtension.h>

#include "ViewProviderAnalysisFeatureExtension.h"


using namespace FemGui;

EXTENSION_PROPERTY_SOURCE(FemGui::ViewProviderFemAnalysisFeatureExtension,
                          Gui::ViewProviderExtension)

ViewProviderFemAnalysisFeatureExtension::ViewProviderFemAnalysisFeatureExtension()
{
    initExtensionType(ViewProviderFemAnalysisFeatureExtension::getExtensionClassTypeId());
}

ViewProviderFemAnalysisFeatureExtension::~ViewProviderFemAnalysisFeatureExtension() = default;

std::vector<App::DocumentObject*>
ViewProviderFemAnalysisFeatureExtension::extensionClaimChildren() const
{
    //    App::DocumentObject* objExt = getExtendedViewProvider()->getObject();
    //    App::DocumentObject* analysis = nullptr;
    //    if (objExt->hasExtension(Fem::FemAnalysisFeatureExtension::getExtensionClassTypeId())) {
    //        analysis =
    //        objExt->getExtension<Fem::FemAnalysisFeatureExtension>()->Analysis.getValue();
    //    }
    //
    printf("aca1\n");
    auto vpClaimed = getExtendedViewProvider()->claimChildren();
    printf("aca2\n");

    //    for (auto obj : vpClaimed) {
    //        if (obj->hasExtension(Fem::FemAnalysisFeatureExtension::getExtensionClassTypeId())) {
    //            obj->getExtension<Fem::FemAnalysisFeatureExtension>()->Analysis.setValue(analysis);
    //        }
    //    }

    return vpClaimed;
}

namespace Gui
{

EXTENSION_PROPERTY_SOURCE_TEMPLATE(FemGui::ViewProviderFemAnalysisFeatureExtensionPython,
                                   FemGui::ViewProviderFemAnalysisFeatureExtension);

template class FemGuiExport ViewProviderExtensionPythonT<ViewProviderFemAnalysisFeatureExtension>;

}  // namespace Gui
