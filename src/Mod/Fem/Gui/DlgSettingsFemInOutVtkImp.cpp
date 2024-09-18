/**************************************************************************
 *   Copyright (c) 2018 FreeCAD Developers                                 *
 *   Author: Bernd Hahnebach <bernd@bimstatik.ch>                          *
 *   Based on src/Mod/Fem/Gui/DlgSettingsFemExportAbaqusCcxImp.cpp         *
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

#include <Gui/Application.h>

#include "DlgSettingsFemInOutVtkImp.h"
#include "ui_DlgSettingsFemInOutVtk.h"


using namespace FemGui;

DlgSettingsFemInOutVtkImp::DlgSettingsFemInOutVtkImp(QWidget* parent)
    : PreferencePage(parent)
    , ui(new Ui_DlgSettingsFemInOutVtk)
{
    ui->setupUi(this);
}

/*
 *  Destroys the object and frees any allocated resources
 */
DlgSettingsFemInOutVtkImp::~DlgSettingsFemInOutVtkImp() = default;

void DlgSettingsFemInOutVtkImp::saveSettings()
{
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Fem/InOutVtk");
    hGrp->SetInt("ImportObject", ui->cb_import_vtk->currentIndex());
    hGrp->SetInt("ExportFemMesh", ui->cb_export_fem_mesh->currentIndex());

    ui->cb_import_vtk->onSave();
    ui->cb_export_fem_mesh->onSave();
}

void DlgSettingsFemInOutVtkImp::loadSettings()
{
    ui->cb_import_vtk->onRestore();
    ui->cb_export_fem_mesh->onRestore();

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
        "User parameter:BaseApp/Preferences/Mod/Fem/InOutVtk");
    int index = hGrp->GetInt("ImportObject", 0);
    // 0 is standard on first initialize, 0 .. vtk res obj, 1 .. FEM mesh obj, 2 .. FreeCAD res obj
    if (index > -1) {
        ui->cb_import_vtk->setCurrentIndex(index);
    }

    index = hGrp->GetInt("ExportFemMesh", 0);
    if (index > -1) {
        ui->cb_export_fem_mesh->setCurrentIndex(index);
    }
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsFemInOutVtkImp::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::LanguageChange) {
        int c_index = ui->cb_import_vtk->currentIndex();
        ui->retranslateUi(this);
        ui->cb_export_fem_mesh->setCurrentIndex(c_index);
        c_index = ui->cb_export_fem_mesh->currentIndex();
        ui->cb_export_fem_mesh->setCurrentIndex(c_index);
    }
    else {
        QWidget::changeEvent(e);
    }
}

#include "moc_DlgSettingsFemInOutVtkImp.cpp"
