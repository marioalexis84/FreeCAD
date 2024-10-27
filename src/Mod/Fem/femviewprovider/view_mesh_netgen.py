# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2024 Mario Passaglia <mpassaglia[at]cbc.uba.ar>         *
# *                                                                         *
# *   This file is part of FreeCAD.                                         *
# *                                                                         *
# *   FreeCAD is free software: you can redistribute it and/or modify it    *
# *   under the terms of the GNU Lesser General Public License as           *
# *   published by the Free Software Foundation, either version 2.1 of the  *
# *   License, or (at your option) any later version.                       *
# *                                                                         *
# *   FreeCAD is distributed in the hope that it will be useful, but        *
# *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
# *   Lesser General Public License for more details.                       *
# *                                                                         *
# *   You should have received a copy of the GNU Lesser General Public      *
# *   License along with FreeCAD. If not, see                               *
# *   <https://www.gnu.org/licenses/>.                                      *
# *                                                                         *
# ***************************************************************************

__title__ = "FreeCAD FEM mesh netgen ViewProvider for the document object"
__author__ = "Mario Passaglia"
__url__ = "https://www.freecad.org"

## @package view_mesh_netgen
#  \ingroup FEM
#  \brief view provider for mesh netgen object

import FreeCAD
import FreeCADGui

import FemGui
from PySide import QtGui
from femtaskpanels import task_mesh_netgen
from femtools.femutils import is_of_type
from femviewprovider import view_base_femobject


class VPMeshNetgen(view_base_femobject.VPBaseFemObject):
    """
    A View Provider for the MeshNetgen object
    """

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/FEM_MeshNetgenFromShape.svg"

    def setEdit(self, vobj, mode):
        # hide all FEM meshes and VTK FemPost* objects
        for obj in vobj.Object.Document.Objects:
            if obj.isDerivedFrom("Fem::FemMeshObject") or obj.isDerivedFrom("Fem::FemPostObject"):
                obj.ViewObject.hide()
        # show the mesh we like to edit
        self.ViewObject.show()
        # show task panel
        taskd = task_mesh_netgen._TaskPanel(self.Object)
        FreeCADGui.Control.showDialog(taskd)
        return True

    def doubleClicked(self, vobj):
        # Group meshing is only active on active analysis
        # we should make sure the analysis the mesh belongs too is active
        gui_doc = FreeCADGui.getDocument(vobj.Object.Document)
        if not gui_doc.getInEdit():
            analysis = vobj.Object.Analysis
            if analysis:
                FemGui.setActiveAnalysis(analysis)
            gui_doc.setEdit(vobj.Object.Name)
        else:
            from PySide.QtGui import QMessageBox

            message = "Active Task Dialog found! Please close this one before opening  a new one!"
            QMessageBox.critical(None, "Error in tree view", message)
            FreeCAD.Console.PrintError(message + "\n")
        return True

    def dumps(self):
        return None

    def loads(self, state):
        return None
