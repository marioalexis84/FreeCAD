# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2025 Mario Passaglia <mpassaglia[at]cbc.uba.ar>         *
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

__title__ = "FreeCAD FEM calculix constraint electrostatic"
__author__ = "Mario Passaglia"
__url__ = "https://www.freecad.org"

import FreeCAD


def get_analysis_types():
    return ["electrostatic"]


def get_sets_name():
    return "constraints_electrostaticpotential_node_sets"


def get_constraint_title():
    return "Fixed electrostatic constraint applied"


def write_meshdata_constraint(f, femobj, pot_obj, ccxwriter):

    if femobj["Object"].BoundaryCondition == "Dirichlet":
        f.write(f"*NSET,NSET={pot_obj.Name}\n")
        for n in femobj["Nodes"]:
            f.write(f"{n},\n")


def get_before_write_meshdata_constraint():
    return ""


def get_after_write_meshdata_constraint():
    return ""


def get_before_write_constraint():
    return ""


def get_after_write_constraint():
    return ""


def write_constraint(f, femobj, pot_obj, ccxwriter):

    # floats read from ccx should use {:.13G}, see comment in writer module

    if pot_obj.BoundaryCondition == "Dirichlet":
        f.write("*BOUNDARY\n")
        f.write("{},11,11,{:.13G}\n".format(pot_obj.Name, pot_obj.Potential.getValueAs("mV").Value))
        f.write("\n")
    elif pot_obj.BoundaryCondition == "Neumann":
        charge_density = pot_obj.SurfaceChargeDensity.getValueAs("A*s/mm^2").Value

        f.write("*DFLUX\n")
        for ref_shape in femobj["ElectricFluxFaces"]:
            # the loop is needed for compatibility reason
            # in deprecated method get_pressure_obj_faces_depreciated
            # the face ids where per ref_shape
            f.write("** " + ref_shape[0] + "\n")
            for face, fno in ref_shape[1]:
                if fno > 0:  # solid mesh face
                    f.write(f"{face},S{fno},{charge_density}\n")
        #                # on shell mesh face: fno == 0
        #                # normal of element face == face normal
        #                elif fno == 0:
        #                    f.write(f"{face},S,{press_rev}\n")
        #                # on shell mesh face: fno == -1
        #                # normal of element face opposite direction face normal
        #                elif fno == -1:
        #                    f.write(f"{face},P,{-1 * press_rev}\n")

        #        f.write("*CFLUX\n")
        #        # CFLUX has to be specified in mW
        #        f.write(
        #            "{},11,{:.13G}\n".format(
        #                pot_obj.Name,
        #                pot_obj.SurfaceChargeDensity.getValueAs("A*s/mm^2").Value,
        #            )
        #        )
        f.write("\n")
