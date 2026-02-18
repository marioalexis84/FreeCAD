# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2026 Mario Passaglia <mpassaglia[at]cbc.uba.ar>         *
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

import sys
import FreeCAD
import Fem


def mesh_from_mesher(femmesh_obj, mesher)
    tool = None
    success = False

    if mesher == "gmsh":
        from femmesh import gmshtools
        tool = gmshtools.GmshTools(femmesh_obj, analysis)

    elif mesher == "netgen":
        from femmesh import netgentools
        tool = netgentools.GmshTools(femmesh_obj, analysis)

    try:
        # generate the mesh
        tool.create_mesh()
        success = True
    except Exception:
        error = sys.exc_info()[1]
        FreeCAD.Console.PrintError(f"Unexpected error when creating mesh: {error}\n")

    return success

def mesh_from_existing(create_nodes, create_elements)
     fem_mesh = Fem.FemMesh()
     control = create_nodes(fem_mesh)
     if not control:
         FreeCAD.Console.PrintError("Error on creating nodes.\n")
     control = create_elements(fem_mesh)
     if not control:
         FreeCAD.Console.PrintError("Error on creating elements.\n")

    return fem_mesh
