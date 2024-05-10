/***************************************************************************
 *   Copyright (c) 2022 Ajinkya Dahale <dahale.a.p@gmail.com>              *
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

#ifndef _PreComp_
#include <QAction>
#include <QMessageBox>
#include <sstream>
#endif

#include <App/Document.h>
#include <Gui/Command.h>
#include <Gui/SelectionObject.h>
#include <Mod/Fem/App/FemConstraintRigidBody.h>

#include "TaskFemConstraintRigidBody.h"
#include "ui_TaskFemConstraintRigidBody.h"


using namespace FemGui;
using namespace Gui;

/* TRANSLATOR FemGui::TaskFemConstraintRigidBody */

TaskFemConstraintRigidBody::TaskFemConstraintRigidBody(
    ViewProviderFemConstraintRigidBody* ConstraintView,
    QWidget* parent)
    : TaskFemConstraintOnBoundary(ConstraintView, parent, "FEM_ConstraintRigidBody")
{  // Note change "RigidBody" in line above to new constraint name
    proxy = new QWidget(this);
    ui = new Ui_TaskFemConstraintRigidBody();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    // create a context menu for the listview of the references
    createDeleteAction(ui->lw_references);
    deleteAction->connect(deleteAction,
                          &QAction::triggered,
                          this,
                          &TaskFemConstraintRigidBody::onReferenceDeleted);

    connect(ui->lw_references,
            &QListWidget::currentItemChanged,
            this,
            &TaskFemConstraintRigidBody::setSelection);
    connect(ui->lw_references,
            &QListWidget::itemClicked,
            this,
            &TaskFemConstraintRigidBody::setSelection);

    // TODO: Relate inputs to property

    this->groupLayout()->addWidget(proxy);

    /* Note: */
    // Get the feature data
    auto pcConstraint = static_cast<Fem::ConstraintRigidBody*>(ConstraintView->getObject());

    const Base::Vector3d& refNode = pcConstraint->ReferenceNode.getValue();
    const Base::Placement& pla = pcConstraint->ConstraintPlacement.getValue();
    Base::Rotation rot = pla.getRotation();
    Base::Vector3d disp = pla.getPosition();
    double rotAngleRad;
    pcConstraint->Rotation.getValue().getValue(rotDir, rotAngleRad);
    Base::Quantity rotAngle(rotAngleRad, QString::fromUtf8("rad"));
    Base::Quantity forceX = pcConstraint->ForceX.getQuantityValue();
    Base::Quantity forceY = pcConstraint->ForceY.getQuantityValue();
    Base::Quantity forceZ = pcConstraint->ForceZ.getQuantityValue();
//    const Base::Vector3d& forceDir = pcConstraint->ForceDirection.getValue();
    Base::Quantity momentX = pcConstraint->MomentX.getQuantityValue();
    Base::Quantity momentY = pcConstraint->MomentY.getQuantityValue();
    Base::Quantity momentZ = pcConstraint->MomentZ.getQuantityValue();
//    const Base::Vector3d& momentDir = pcConstraint->MomentDirection.getValue();

    std::vector<App::DocumentObject*> Objects = pcConstraint->References.getValues();
    std::vector<std::string> SubElements = pcConstraint->References.getSubValues();

    // Fill data into dialog elements
    ui->qsb_ref_node_x->setValue(refNode.x);
    ui->qsb_ref_node_y->setValue(refNode.y);
    ui->qsb_ref_node_z->setValue(refNode.z);
    ui->qsb_ref_node_x->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("ReferenceNode.x")));
    ui->qsb_ref_node_y->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("ReferenceNode.y")));
    ui->qsb_ref_node_z->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("ReferenceNode.z")));
    ui->qsb_ref_node_x->setMinimum(-FLOAT_MAX);
    ui->qsb_ref_node_x->setMaximum(FLOAT_MAX);
    ui->qsb_ref_node_y->setMinimum(-FLOAT_MAX);
    ui->qsb_ref_node_y->setMaximum(FLOAT_MAX);
    ui->qsb_ref_node_z->setMinimum(-FLOAT_MAX);
    ui->qsb_ref_node_z->setMaximum(FLOAT_MAX);

    ui->qsb_disp_x->setValue(disp.x);
    ui->qsb_disp_y->setValue(disp.y);
    ui->qsb_disp_z->setValue(disp.z);
    ui->qsb_disp_x->bind(App::ObjectIdentifier::parse(pcConstraint, std::string("Displacement.x")));
    ui->qsb_disp_y->bind(App::ObjectIdentifier::parse(pcConstraint, std::string("Displacement.y")));
    ui->qsb_disp_z->bind(App::ObjectIdentifier::parse(pcConstraint, std::string("Displacement.z")));
    ui->qsb_disp_x->setMinimum(-FLOAT_MAX);
    ui->qsb_disp_x->setMaximum(FLOAT_MAX);
    ui->qsb_disp_y->setMinimum(-FLOAT_MAX);
    ui->qsb_disp_y->setMaximum(FLOAT_MAX);
    ui->qsb_disp_z->setMinimum(-FLOAT_MAX);
    ui->qsb_disp_z->setMaximum(FLOAT_MAX);

    ui->spb_rot_axis_x->setValue(rotDir.x);
    ui->spb_rot_axis_y->setValue(rotDir.y);
    ui->spb_rot_axis_z->setValue(rotDir.z);
    ui->qsb_rot_angle->setValue(rotAngle.getValueAs(Base::Quantity::Degree));
    ui->spb_rot_axis_x->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("Rotation.Axis.x")));
    ui->spb_rot_axis_y->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("Rotation.Axis.y")));
    ui->spb_rot_axis_z->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("Rotation.Axis.z")));
    ui->qsb_rot_angle->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("Rotation.Angle")));
    ui->spb_rot_axis_x->setMinimum(-FLOAT_MAX);
    ui->spb_rot_axis_x->setMaximum(FLOAT_MAX);
    ui->spb_rot_axis_y->setMinimum(-FLOAT_MAX);
    ui->spb_rot_axis_y->setMaximum(FLOAT_MAX);
    ui->spb_rot_axis_z->setMinimum(-FLOAT_MAX);
    ui->spb_rot_axis_z->setMaximum(FLOAT_MAX);
    ui->qsb_rot_angle->setMinimum(-FLOAT_MAX);
    ui->qsb_rot_angle->setMaximum(FLOAT_MAX);

    ui->qsb_force_x->setValue(forceDir.x);
    ui->qsb_force_y->setValue(forceDir.y);
    ui->qsb_force_z->setValue(forceDir.z);
    ui->spb_force_x->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("ForceDirection.x")));
    ui->qsb_force_y->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("ForceDirection.y")));
    ui->qsb_force_z->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("ForceDirection.z")));
//    ui->qsb_force->bind(pcConstraint->Force);
    ui->qsb_force_x->setMinimum(-FLOAT_MAX);
    ui->qsb_force_x->setMaximum(FLOAT_MAX);
    ui->qsb_force_y->setMinimum(-FLOAT_MAX);
    ui->qsb_force_y->setMaximum(FLOAT_MAX);
    ui->qsb_force_z->setMinimum(-FLOAT_MAX);
    ui->qsb_force_z->setMaximum(FLOAT_MAX);
//    ui->qsb_force->setMinimum(-FLOAT_MAX);
//    ui->qsb_force->setMaximum(FLOAT_MAX);

    ui->qsb_moment_x->setValue(momentDir.x);
    ui->qsb_moment_y->setValue(momentDir.y);
    ui->qsb_moment_z->setValue(momentDir.z);
//    ui->qsb_moment->setValue(moment);
    ui->qsb_moment_x->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("MomentDirection.x")));
    ui->qsb_moment_y->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("MomentDirection.y")));
    ui->qsb_moment_z->bind(
        App::ObjectIdentifier::parse(pcConstraint, std::string("MomentDirection.z")));
    ui->qsb_moment_x->setMinimum(-FLOAT_MAX);
    ui->qsb_moment_x->setMaximum(FLOAT_MAX);
    ui->qsb_moment_y->setMinimum(-FLOAT_MAX);
    ui->qsb_moment_y->setMaximum(FLOAT_MAX);
    ui->qsb_moment_z->setMinimum(-FLOAT_MAX);
    ui->qsb_moment_z->setMaximum(FLOAT_MAX);
//    ui->qsb_moment->setMinimum(-FLOAT_MAX);
//    ui->qsb_moment->setMaximum(FLOAT_MAX);
//    ui->qsb_moment->bind(pcConstraint->Moment);

    QStringList modeList;

    App::PropertyEnumeration* transMode = &pcConstraint->XTranslationalMode;
    for (auto item : transMode->getEnumVector()) {
        modeList << QString::fromUtf8(item.c_str());
    }
    ui->cb_x_trans_mode->addItems(modeList);
    ui->cb_y_trans_mode->addItems(modeList);
    ui->cb_z_trans_mode->addItems(modeList);
    ui->cb_x_trans_mode->setCurrentIndex(pcConstraint->XTranslationalMode.getValue());
    ui->cb_y_trans_mode->setCurrentIndex(pcConstraint->YTranslationalMode.getValue());
    ui->cb_z_trans_mode->setCurrentIndex(pcConstraint->ZTranslationalMode.getValue());

    modeList.clear();
    App::PropertyEnumeration* rotMode = &pcConstraint->XRotationalMode;
    for (auto item : rotMode->getEnumVector()) {
        modeList << QString::fromUtf8(item.c_str());
    }
    ui->cb_x_rot_mode->addItems(modeList);
    ui->cb_y_rot_mode->addItems(modeList);
    ui->cb_z_rot_mode->addItems(modeList);
    ui->cb_x_rot_mode->setCurrentIndex(pcConstraint->XRotationalMode.getValue());
    ui->cb_y_rot_mode->setCurrentIndex(pcConstraint->YRotationalMode.getValue());
    ui->cb_z_rot_mode->setCurrentIndex(pcConstraint->ZRotationalMode.getValue());


    ui->lw_references->clear();
    for (std::size_t i = 0; i < Objects.size(); i++) {
        ui->lw_references->addItem(makeRefText(Objects[i], SubElements[i]));
    }
    if (!Objects.empty()) {
        ui->lw_references->setCurrentRow(0, QItemSelectionModel::ClearAndSelect);
    }

    // Selection buttons
    buttonGroup->addButton(ui->btnAdd, (int)SelectionChangeModes::refAdd);
    buttonGroup->addButton(ui->btnRemove, (int)SelectionChangeModes::refRemove);

    updateUI();
}

TaskFemConstraintRigidBody::~TaskFemConstraintRigidBody()
{
    delete ui;
}

void TaskFemConstraintRigidBody::updateUI()
{
    if (ui->lw_references->model()->rowCount() == 0) {
        // Go into reference selection mode if no reference has been selected yet
        onButtonReference(true);
        return;
    }
}

void TaskFemConstraintRigidBody::addToSelection()
{
    std::vector<Gui::SelectionObject> selection =
        Gui::Selection().getSelectionEx();  // gets vector of selected objects of active document
    if (selection.empty()) {
        QMessageBox::warning(this, tr("Selection error"), tr("Nothing selected!"));
        return;
    }
    Fem::ConstraintRigidBody* pcConstraint =
        static_cast<Fem::ConstraintRigidBody*>(ConstraintView->getObject());
    std::vector<App::DocumentObject*> Objects = pcConstraint->References.getValues();
    std::vector<std::string> SubElements = pcConstraint->References.getSubValues();

    for (std::vector<Gui::SelectionObject>::iterator it = selection.begin(); it != selection.end();
         ++it) {  // for every selected object
        if (!it->isObjectTypeOf(Part::Feature::getClassTypeId())) {
            QMessageBox::warning(this, tr("Selection error"), tr("Selected object is not a part!"));
            return;
        }
        std::vector<std::string> subNames = it->getSubNames();
        App::DocumentObject* obj =
            ConstraintView->getObject()->getDocument()->getObject(it->getFeatName());
        for (size_t subIt = 0; subIt < (subNames.size());
             ++subIt) {  // for every selected sub element
            bool addMe = true;
            for (std::vector<std::string>::iterator itr =
                     std::find(SubElements.begin(), SubElements.end(), subNames[subIt]);
                 itr != SubElements.end();
                 itr = std::find(++itr,
                                 SubElements.end(),
                                 subNames[subIt])) {  // for every sub element in selection that
                                                      // matches one in old list
                if (obj
                    == Objects[std::distance(
                        SubElements.begin(),
                        itr)]) {  // if selected sub element's object equals the one in old list
                                  // then it was added before so don't add
                    addMe = false;
                }
            }
            // limit constraint such that only vertexes or faces or edges can be used depending on
            // what was selected first
            std::string searchStr;
            if (subNames[subIt].find("Vertex") != std::string::npos) {
                searchStr = "Vertex";
            }
            else if (subNames[subIt].find("Edge") != std::string::npos) {
                searchStr = "Edge";
            }
            else {
                searchStr = "Face";
            }
            for (size_t iStr = 0; iStr < (SubElements.size()); ++iStr) {
                if (SubElements[iStr].find(searchStr) == std::string::npos) {
                    QString msg = tr(
                        "Only one type of selection (vertex,face or edge) per constraint allowed!");
                    QMessageBox::warning(this, tr("Selection error"), msg);
                    addMe = false;
                    break;
                }
            }
            if (addMe) {
                QSignalBlocker block(ui->lw_references);
                Objects.push_back(obj);
                SubElements.push_back(subNames[subIt]);
                ui->lw_references->addItem(makeRefText(obj, subNames[subIt]));
            }
        }
    }
    // Update UI
    pcConstraint->References.setValues(Objects, SubElements);
    updateUI();
}

void TaskFemConstraintRigidBody::removeFromSelection()
{
    std::vector<Gui::SelectionObject> selection =
        Gui::Selection().getSelectionEx();  // gets vector of selected objects of active document
    if (selection.empty()) {
        QMessageBox::warning(this, tr("Selection error"), tr("Nothing selected!"));
        return;
    }
    Fem::ConstraintRigidBody* pcConstraint =
        static_cast<Fem::ConstraintRigidBody*>(ConstraintView->getObject());
    std::vector<App::DocumentObject*> Objects = pcConstraint->References.getValues();
    std::vector<std::string> SubElements = pcConstraint->References.getSubValues();
    std::vector<size_t> itemsToDel;
    for (std::vector<Gui::SelectionObject>::iterator it = selection.begin(); it != selection.end();
         ++it) {  // for every selected object
        if (!it->isObjectTypeOf(Part::Feature::getClassTypeId())) {
            QMessageBox::warning(this, tr("Selection error"), tr("Selected object is not a part!"));
            return;
        }
        const std::vector<std::string>& subNames = it->getSubNames();
        App::DocumentObject* obj = it->getObject();

        for (size_t subIt = 0; subIt < (subNames.size());
             ++subIt) {  // for every selected sub element
            for (std::vector<std::string>::iterator itr =
                     std::find(SubElements.begin(), SubElements.end(), subNames[subIt]);
                 itr != SubElements.end();
                 itr = std::find(++itr,
                                 SubElements.end(),
                                 subNames[subIt])) {  // for every sub element in selection that
                                                      // matches one in old list
                if (obj
                    == Objects[std::distance(
                        SubElements.begin(),
                        itr)]) {  // if selected sub element's object equals the one in old list
                                  // then it was added before so mark for deletion
                    itemsToDel.push_back(std::distance(SubElements.begin(), itr));
                }
            }
        }
    }
    std::sort(itemsToDel.begin(), itemsToDel.end());
    while (!itemsToDel.empty()) {
        Objects.erase(Objects.begin() + itemsToDel.back());
        SubElements.erase(SubElements.begin() + itemsToDel.back());
        itemsToDel.pop_back();
    }
    // Update UI
    {
        QSignalBlocker block(ui->lw_references);
        ui->lw_references->clear();
        for (unsigned int j = 0; j < Objects.size(); j++) {
            ui->lw_references->addItem(makeRefText(Objects[j], SubElements[j]));
        }
    }
    pcConstraint->References.setValues(Objects, SubElements);
    updateUI();
}

void TaskFemConstraintRigidBody::onReferenceDeleted()
{
    TaskFemConstraintRigidBody::removeFromSelection();
}

const std::string TaskFemConstraintRigidBody::getReferences() const
{
    int rows = ui->lw_references->model()->rowCount();
    std::vector<std::string> items;
    for (int r = 0; r < rows; r++) {
        items.push_back(ui->lw_references->item(r)->text().toStdString());
    }
    return TaskFemConstraint::getReferences(items);
}

Base::Vector3d TaskFemConstraintRigidBody::getReferenceNode() const
{
    double x = ui->qsb_ref_node_x->rawValue();
    double y = ui->qsb_ref_node_y->rawValue();
    double z = ui->qsb_ref_node_z->rawValue();

    return Base::Vector3d(x, y, z);
}

Base::Vector3d TaskFemConstraintRigidBody::getDisplacement() const
{
    double x = ui->qsb_disp_x->rawValue();
    double y = ui->qsb_disp_y->rawValue();
    double z = ui->qsb_disp_z->rawValue();

    return Base::Vector3d(x, y, z);
}

Base::Rotation TaskFemConstraintRigidBody::getRotation() const
{
    double x = ui->qsb_rot_x->value().getValue();
    double y = ui->qsb_rot_y->value().getValue();
    double z = ui->qsb_rot_z->value().getValue();

    Base::Quantity angleDeg(sqrt(x*x + y*y + z*z), QString::fromUtf8("deg"));
    double angle = angleDeg.getValueAs(Base::Quantity::Radian);

    return Base::Rotation(Base::Vector3d(x, y, z), angle);
}

std::vector<std::string> TaskFemConstraintRigidBody::getForce() const
{
    std::string x = ui->qsb_force_x->value().getSafeUserString().toStdString();
    std::string y = ui->qsb_force_y->value().getSafeUserString().toStdString();
    std::string z = ui->qsb_force_z->value().getSafeUserString().toStdString();

    return {x, y, z};
}

std::vector<std::string> TaskFemConstraintRigidBody::getMoment() const
{
    std::string x = ui->qsb_moment_x->value().getSafeUserString().toStdString();
    std::string y = ui->qsb_moment_y->value().getSafeUserString().toStdString();
    std::string z = ui->qsb_moment_z->value().getSafeUserString().toStdString();

    return std::vector<Base::Quantity>({x, y, z});
}

//std::string TaskFemConstraintRigidBody::getForce() const
//{
//    return ui->qsb_force->value().getSafeUserString().toStdString();
//}
//
//std::string TaskFemConstraintRigidBody::getMoment() const
//{
//    return ui->qsb_moment->value().getSafeUserString().toStdString();
//}

std::vector<std::string> TaskFemConstraintRigidBody::getTranslationalMode() const
{
    std::vector<std::string> transModes(3);
    transModes[0] = ui->cb_x_trans_mode->currentText().toStdString();
    transModes[1] = ui->cb_y_trans_mode->currentText().toStdString();
    transModes[2] = ui->cb_z_trans_mode->currentText().toStdString();

    return transModes;
}

std::vector<std::string> TaskFemConstraintRigidBody::getRotationalMode() const
{
    std::vector<std::string> rotModes(3);
    rotModes[0] = ui->cb_x_rot_mode->currentText().toStdString();
    rotModes[1] = ui->cb_y_rot_mode->currentText().toStdString();
    rotModes[2] = ui->cb_z_rot_mode->currentText().toStdString();

    return rotModes;
}

bool TaskFemConstraintRigidBody::event(QEvent* e)
{
    return TaskFemConstraint::KeyEvent(e);
}

void TaskFemConstraintRigidBody::changeEvent(QEvent*)
{}

void TaskFemConstraintRigidBody::clearButtons(const SelectionChangeModes notThis)
{
    if (notThis != SelectionChangeModes::refAdd) {
        ui->btnAdd->setChecked(false);
    }
    if (notThis != SelectionChangeModes::refRemove) {
        ui->btnRemove->setChecked(false);
    }
}

//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgFemConstraintRigidBody::TaskDlgFemConstraintRigidBody(
    ViewProviderFemConstraintRigidBody* ConstraintView)
{
    this->ConstraintView = ConstraintView;
    assert(ConstraintView);
    this->parameter = new TaskFemConstraintRigidBody(ConstraintView);

    Content.push_back(parameter);
}

//==== calls from the TaskView ===============================================================

void TaskDlgFemConstraintRigidBody::open()
{
    // a transaction is already open at creation time of the panel
    if (!Gui::Command::hasPendingCommand()) {
        QString msg = QObject::tr("Constraint RigidBody");
        Gui::Command::openCommand((const char*)msg.toUtf8());
        ConstraintView->setVisible(true);
        Gui::Command::doCommand(
            Gui::Command::Doc,
            ViewProviderFemConstraint::gethideMeshShowPartStr(
                (static_cast<Fem::Constraint*>(ConstraintView->getObject()))->getNameInDocument())
                .c_str());  // OvG: Hide meshes and show parts
    }
}

bool TaskDlgFemConstraintRigidBody::accept()
{
    std::string name = ConstraintView->getObject()->getNameInDocument();
    const TaskFemConstraintRigidBody* parameters =
        static_cast<const TaskFemConstraintRigidBody*>(parameter);
    try {
        Base::Vector3d ref = parameters->getReferenceNode();
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.ReferenceNode = App.Vector(%f, %f, %f)",
                                name.c_str(),
                                ref.x,
                                ref.y,
                                ref.z);

        Base::Vector3d disp = parameters->getDisplacement();
//        Gui::Command::doCommand(Gui::Command::Doc,
//                                "App.ActiveDocument.%s.Displacement = App.Vector(%f, %f, %f)",
//                                name.c_str(),
//                                disp.x,
//                                disp.y,
//                                disp.z);

        Base::Rotation rot = parameters->getRotation();
        Base::Vector3d axis;
        double angle;
        rot.getValue(axis, angle);
//        Gui::Command::doCommand(
//            Gui::Command::Doc,
//            "App.ActiveDocument.%s.Rotation = App.Rotation(App.Vector(%f,% f, %f), Radian=%f)",
//            name.c_str(),
//            axis.x,
//            axis.y,
//            axis.z,
//            angle);
        Gui::Command::doCommand(
            Gui::Command::Doc,
            "App.ActiveDocument.%s.ConstraintPlacement = App.Placement(App.Vector(%f, %f, %f), App.Rotation(App.Vector(%f,% f, %f), Radian=%f))",
            name.c_str(),
            disp.x,
            disp.y,
            disp.z,
            axis.x,
            axis.y,
            axis.z,
            angle);

//        Gui::Command::doCommand(Gui::Command::Doc,
//                                "App.ActiveDocument.%s.Force = \"%s\"",
//                                name.c_str(),
//                                parameters->getForce().c_str());
//
        auto force = parameters->getForce();
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.ForceX = \"%s\"",
                                name.c_str(),
                                force[0]);
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.ForceY = \"%s\"",
                                name.c_str(),
                                force[1]);
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.ForceZ = \"%s\"",
                                name.c_str(),
                                force[2]);

        auto moment = parameters->getMoment();
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.MomentX = \"%s\"",
                                name.c_str(),
                                moment[0]);
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.MomentY = \"%s\"",
                                name.c_str(),
                                moment[1]);
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.MomentZ = \"%s\"",
                                name.c_str(),
                                moment[2]);
//        Gui::Command::doCommand(Gui::Command::Doc,
//                                "App.ActiveDocument.%s.ForceDirection = App.Vector(%f, %f, %f)",
//                                name.c_str(),
//                                force.x,
//                                force.y,
//                                force.z);

//        Gui::Command::doCommand(Gui::Command::Doc,
//                                "App.ActiveDocument.%s.Moment = \"%s\"",
//                                name.c_str(),
//                                parameters->getMoment().c_str());
//
//        Base::Vector3d momentDir = parameters->getMomentDirection();
//        Gui::Command::doCommand(Gui::Command::Doc,
//                                "App.ActiveDocument.%s.MomentDirection = App.Vector(%f, %f, %f)",
//                                name.c_str(),
//                                momentDir.x,
//                                momentDir.y,
//                                momentDir.z);

        auto transModes = parameters->getRotationalMode();
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.XTranslationalMode = \"%s\"",
                                name.c_str(),
                                transModes[0].c_str());
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.YTranslationalMode = \"%s\"",
                                name.c_str(),
                                transModes[1].c_str());
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.ZTranslationalMode = \"%s\"",
                                name.c_str(),
                                transModes[2].c_str());

        auto rotModes = parameters->getRotationalMode();
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.XRotationalMode = \"%s\"",
                                name.c_str(),
                                rotModes[0].c_str());
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.YRotationalMode = \"%s\"",
                                name.c_str(),
                                rotModes[1].c_str());
        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.ZRotationalMode = \"%s\"",
                                name.c_str(),
                                rotModes[2].c_str());

        Gui::Command::doCommand(Gui::Command::Doc,
                                "App.ActiveDocument.%s.Scale = %s",
                                name.c_str(),
                                parameters->getScale().c_str());
    }
    catch (const Base::Exception& e) {
        QMessageBox::warning(parameter, tr("Input error"), QString::fromLatin1(e.what()));
        return false;
    }
    return TaskDlgFemConstraint::accept();
}

bool TaskDlgFemConstraintRigidBody::reject()
{
    Gui::Command::abortCommand();
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.activeDocument().resetEdit()");
    Gui::Command::updateActive();

    return true;
}

#include "moc_TaskFemConstraintRigidBody.cpp"
