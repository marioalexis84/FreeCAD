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

#include "FemConstraintRigidBody.h"


using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintRigidBody, Fem::Constraint)

const char* ConstraintRigidBody::loadModeEnum[] = {"Displacement-Rotation",
                                                   "Force-Moment",
                                                   "Displacement-Moment",
                                                   "Force-Rotation",
                                                   nullptr};

const char* ConstraintRigidBody::freeModeEnum[] =
    {"None", "X", "Y", "Z", "XY", "XZ", "YZ", "All", nullptr};

ConstraintRigidBody::ConstraintRigidBody()
{
    ADD_PROPERTY_TYPE(ReferenceNode,
                      (0.0, 0.0, 0.0),
                      "ConstraintRigidBody",
                      App::Prop_Output,
                      "Reference node position");
    ADD_PROPERTY_TYPE(Displacement,
                      (0.0, 0.0, 0.0),
                      "ConstraintRigidBody",
                      App::Prop_Output,
                      "Reference node displacement");
    ADD_PROPERTY_TYPE(Rotation,
                      (Base::Rotation(0.0, 0.0, 0.0, 0.0)),
                      "ConstraintRigidBody",
                      App::Prop_Output,
                      "Reference node rotation");
    ADD_PROPERTY_TYPE(Force, (0.0), "ConstraintRigidBody", App::Prop_Output, "Applied force");
    ADD_PROPERTY_TYPE(ForceDirection,
                      (0.0, 0.0, 0.0),
                      "ConstraintRigidBody",
                      App::Prop_Output,
                      "Direction of applied force");
    ADD_PROPERTY_TYPE(Moment, (0.0), "ConstraintRigidBody", App::Prop_Output, "Applied moment");
    ADD_PROPERTY_TYPE(MomentDirection,
                      (0.0, 0.0, 0.0),
                      "ConstraintRigidBody",
                      App::Prop_Output,
                      "Direction of applied moment");
    ADD_PROPERTY_TYPE(FreeTranslationalMode,
                      ("None"),
                      "ConstraintRigidBody",
                      App::Prop_Output,
                      "Free displacement/force  mode");
    ADD_PROPERTY_TYPE(FreeRotationalMode,
                      ("None"),
                      "ConstraintRigidBody",
                      App::Prop_Output,
                      "Free rotation/moment mode");
    ADD_PROPERTY_TYPE(LoadMode,
                      ("Displacement-Rotation"),
                      "ConstraintRigidBody",
                      App::Prop_Output,
                      "Load/boundary condition mode");

    FreeTranslationalMode.setEnums(freeModeEnum);
    FreeRotationalMode.setEnums(freeModeEnum);
    LoadMode.setEnums(loadModeEnum);
}

App::DocumentObjectExecReturn* ConstraintRigidBody::execute()
{
    return Constraint::execute();
}

void ConstraintRigidBody::onChanged(const App::Property* prop)
{
    Constraint::onChanged(prop);
}
