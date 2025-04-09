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

#include "PostDataObjectPy.h"


using namespace Gui;

PostDataObjectPy::PostDataObjectPy(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds)
    : Py::PythonClass<PostDataObjectPy>::PythonClass(self, args, kwds)
{
    const char* name;
    if (!PyArg_ParseTuple(args.ptr(), "")) {
        throw Py::Exception();
    }
}

PostDataObjectPy::~PostDataObjectPy() = default;

Py::Object PostDataObjectPy::getDescription()
{
    if (data) {
        Py::String descr = data->GetObjectDescription();
        return descr;
    }

    return Py::None();
}
PYCXX_NOARGS_METHOD_DECL(PostDataObjectPy, getDescription)

void PostDataObjectPy::init_type()
{
    //    Base::PythonTypeExt ext(behaviors());

    behaviors().name("Fem.PostDataObject");
    behaviors().doc("VTK data object");
    behaviors().supportRepr();
    behaviors().supportGetattro();
    //    behaviors().supportSetattro();
    //    ext.set_tp_descr_get(&PostDataObjectPy::descriptorGetter);
    //    ext.set_tp_descr_set(&PostDataObjectPy::descriptorSetter);
    PYCXX_ADD_NOARGS_METHOD(getCommand, getDescription, "VTK data object Description");

    behaviors().readyType();
}

// PyObject* PostDataObjectPy::descriptorGetter(PyObject* self, PyObject* /*obj*/, PyObject*
// /*type*/)
//{
//     auto cmdAction = Py::PythonClassObject<PostDataObjectPy>(self).getCxxObject();
//
//     return Py::new_reference_to(cmdAction->getAction());
// }
//
// int PostDataObjectPy::descriptorSetter(PyObject* /*self*/, PyObject* /*obj*/, PyObject* value)
//{
//     if (value) {
//         PyErr_SetString(PyExc_AttributeError, "Can't overwrite command action");
//     }
//     else {
//         PyErr_SetString(PyExc_AttributeError, "Can't delete command action");
//     }
//
//     return -1;
// }

Py::Object PostDataObjectPy::repr()
{
    std::stringstream s;
    s /*<< this->cmdName*/ << "VTK data object";

    return Py::String(s.str());
}

Py::Object PostDataObjectPy::getattro(const Py::String& attr_)
{
    //    std::string attr = static_cast<std::string>(attr_);
    Py::Dict d;
    //    d["info"] = Py::String(this->data->GetObjectDescription());
    if (attr == "__dict__") {
        return d;
    }
    //    else if (attr == "name") {
    //        return d["name"];
    //    }
    //    else {
    return genericGetAttro(attr_);
    //    }
}

void PostDataObjectPy::setData(vtkSmartPointer<vtkDataObject> obj)
{
    data = obj;
}
// int PostDataObjectPy::setattro(const Py::String& attr_, const Py::Object& value)
//{
////    std::string attr = static_cast<std::string>(attr_);
////    if (attr == "name" && value.isString()) {
////        cmdName = static_cast<std::string>(Py::String(value));
////        cmd = Application::Instance->commandManager().getCommandByName(cmdName.c_str());
////    }
////    else {
////        return genericSetAttro(attr_, value);
////    }
//
//    return 0;
//}
