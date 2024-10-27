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

#include "FemAnalysis.h"
#include "FemAnalysisFeatureExtension.h"


using namespace App;
using namespace Fem;

EXTENSION_PROPERTY_SOURCE(Fem::FemAnalysisFeatureExtension, App::DocumentObjectExtension);

FemAnalysisFeatureExtension::FemAnalysisFeatureExtension()
{
    initExtensionType(FemAnalysisFeatureExtension::getExtensionClassTypeId());

    EXTENSION_ADD_PROPERTY_TYPE(Analysis,
                                (nullptr),
                                "Base",
                                Prop_Hidden,
                                "Analysis in which this feature is contained");
}

FemAnalysisFeatureExtension::~FemAnalysisFeatureExtension() = default;

void FemAnalysisFeatureExtension::onExtendedDocumentRestored()
{
    if (!Analysis.getValue()) {
        auto obj = static_cast<const App::DocumentObject*>(getExtendedContainer());
        auto objInList = obj->getInListRecursive();
        auto it = std::find_if(objInList.begin(), objInList.end(), [](App::DocumentObject* item) {
            return item->isDerivedFrom<FemAnalysis>() ? true : false;
        });
        if (it != objInList.end()) {
            Analysis.setValue(*it);
        }
    }
}

namespace App
{

EXTENSION_PROPERTY_SOURCE_TEMPLATE(Fem::FemAnalysisFeatureExtensionPython,
                                   Fem::FemAnalysisFeatureExtension);

template class FemExport
    ExtensionPythonT<FemAnalysisFeatureExtensionPythonT<FemAnalysisFeatureExtension>>;

}  // namespace App
