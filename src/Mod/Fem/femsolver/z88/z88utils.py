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

__title__ = "Utils for Z88 solver"
__author__ = "Mario Passaglia"
__url__ = "https://www.freecad.org"


# smesh type string names from type
smesh_type_names = {
    0: "Node",
    1: "0D",
    2: "Edge",
    3: "Quad_Edge",
    4: "Triangle",
    5: "Quad_Triangle",
    6: "BiQuad_Triangle",
    7: "Quadrangle",
    8: "Quad_Quadrangle",
    9: "BiQuad_Quadrangle",
    10: "Polygon",
    11: "Quad_Polygon",
    12: "Tetra",
    13: "Quad_Tetra",
    14: "Pyramid",
    15: "Quad_Pyramid",
    16: "Hexa",
    17: "Quad_Hexa",
    18: "TriQuad_Hexa",
    19: "Penta",
    20: "Quad_Penta",
    21: "Hexagonal_Prism",
    22: "Polyhedra",
    23: "Quad_Polyhedra",
    24: "Ball",
    25: "SMDSEntity_Last",
}

# dof by Z88 element type
z88_dof = {
    1: 3,
    2: 6,
    3: 2,
    4: 3,
    5: 6,
    6: 2,
    7: 2,
    8: 2,
    9: 2,
    10: 3,
    11: 2,
    12: 2,
    13: 3,
    14: 2,
    15: 2,
    16: 3,
    17: 3,
    18: 3,
    19: 3,
    20: 3,
    21: 3,
    22: 3,
    23: 6,
    24: 6,
    25: 6,
}

smesh_volume_type_from_nodes = {
    4: 12,
    10: 13,
    5: 14,
    9: 15,
    8: 16,
    20: 17,
    6: 19,
    15: 20,
}

smesh_face_type_from_nodes = {
    3: 4,
    6: 5,
    4: 7,
    8: 8,
}

smesh_edge_type_from_nodes = {
    2: 2,
    3: 3,
}

# Z88 nodes order from smesh type
smesh_to_z88_order = {
    # edges:
    2: [0, 1],
    # faces
    5: [0, 1, 2, 3, 4, 5],
    8: [0, 1, 2, 3, 4, 5, 6, 7],
    # volumes
    12: [0, 2, 1, 3],
    13: [0, 2, 1, 3, 6, 5, 4, 9, 8, 7],
    16: [0, 3, 7, 4, 1, 2, 6, 5],
    17: [6, 5, 4, 7, 2, 1, 0, 3, 13, 12, 15, 14, 9, 8, 11, 10, 18, 17, 16, 19],
}


def smesh_to_z88_type(solver):
    """Get Z88 type from smesh type, according to element model"""
    seg2 = 25
    if solver.ExcludeBendingStiffness:
        seg2 = 4

    quad8 = 23
    tria6 = 24
    match solver.ModelSpace:
        case "plane stress":
            quad8 = 7
            tria6 = 14
        case "axisymmetric":
            quad8 = 8
            tria6 = 15
        case "plate":
            quad8 = 20
            tria6 = 18

    return {
        2: seg2,
        5: tria6,
        8: quad8,
        12: 17,
        13: 16,
        16: 1,
        17: 10,
    }


def define_masks():
    """Get element masks"""
    # edge masks to search edges from face elements (in smesh order)
    mask_tria3 = {0b011: [0, 1], 0b110: [1, 2], 0b101: [2, 0]}
    mask_tria6 = {0b001011: [0, 1, 3], 0b010110: [1, 2, 4], 0b100101: [2, 0, 5]}
    mask_quad8 = {
        0b00010011: [0, 1, 4],
        0b00100110: [1, 2, 5],
        0b01001100: [2, 3, 6],
        0b10001001: [3, 0, 7],
    }

    # face masks to search faces from volume elements (in smesh order)
    mask_tetra4 = {
        0b0111: [0, 2, 1],
        0b1011: [2, 0, 3],
        0b1101: [0, 1, 3],
        0b1110: [1, 2, 3],
    }
    mask_tetra10 = {
        0b0001110111: [0, 2, 1, 6, 5, 4],
        0b0110011011: [2, 0, 3, 6, 9, 8],
        0b1011001101: [0, 1, 3, 4, 7, 9],
        0b1100101110: [1, 2, 3, 5, 8, 7],
    }
    mask_hexa8 = {
        0b00001111: [0, 4, 5, 1],
        0b11110000: [3, 2, 6, 7],
        0b01100110: [5, 4, 7, 6],
        0b11001100: [1, 5, 6, 2],
        0b10011001: [0, 1, 2, 3],
        0b00110011: [4, 0, 3, 7],
    }
    mask_hexa20 = {
        0b00000000111100001111: [6, 5, 4, 7, 13, 12, 15, 14],
        0b00001111000011110000: [2, 3, 0, 1, 10, 11, 8, 9],
        0b01100010001001100110: [5, 1, 0, 4, 17, 8, 16, 12],
        0b11000100010011001100: [4, 0, 3, 7, 16, 11, 19, 15],
        0b10011000100010011001: [6, 7, 3, 2, 14, 19, 10, 18],
        0b00110001000100110011: [6, 2, 1, 5, 18, 9, 17, 13],
    }

    return {
        "tria3": mask_tria3,
        "tria6": mask_tria6,
        "quad8": mask_quad8,
        "tetra4": mask_tetra4,
        "tetra10": mask_tetra10,
        "hexa8": mask_hexa8,
        "hexa20": mask_hexa20,
    }


# for solver parameter file Z88man.txt
z88_man_template = """DYNAMIC START
---------------------------------------------------------------------------
Z88
---------------------------------------------------------------------------

---------------------------------------------------------------------------
GLOBAL
---------------------------------------------------------------------------

GLOBAL START
    IBFLAG  {param_ibflag}
    IPFLAG  {param_ipflag}
    IHFLAG  {param_ihflag}
    SIMCASE 1
GLOBAL END

---------------------------------------------------------------------------
LINEAR SOLVER
---------------------------------------------------------------------------

SOLVER START
    MAXIT   {param_maxit}
    EPS     {param_eps}
    RALPHA  {param_ralpha}
    ROMEGA  {param_romega}
SOLVER END

---------------------------------------------------------------------------
STRESS
---------------------------------------------------------------------------

STRESS START
    KDFLAG  {param_kdflag}
    ISFLAG  {param_isflag}
STRESS END

DYNAMIC END
"""

# for memory parameter file z88.dyn
z88_dyn_template = """DYNAMIC START
---------------------------------------------------------------------------
Z88                                    Z88
---------------------------------------------------------------------------

---------------------------------------------------------------------------
LANGUAGE                   SPRACHE
---------------------------------------------------------------------------
ENGLISH

---------------------------------------------------------------------------
Entries for mesh generator Z88N        Daten fuer Netzgenerator
---------------------------------------------------------------------------
NET START
    MAXSE   40000
    MAXESS  800
    MAXKSS  4000
    MAXAN   15
NET END

---------------------------------------------------------------------------
Common entries for all modules         gemeinsame Daten fuer alle Module
---------------------------------------------------------------------------

COMMON START
    MAXGS   {param_maxgs}
    MAXKOI  {param_maxkoi}
    MAXK    60000
    MAXE    300000
    MAXNFG  200000
    MAXMAT  32
    MAXPEL  32
    MAXJNT  32
    MAXPR   10000
    MAXRBD  15000
    MAXIEZ  6000000
    MAXGP   2000000
COMMON END

---------------------------------------------------------------------------
Entries for Cuthill-McKee Z88H         Daten fuer Cuthill- McKee Programm
---------------------------------------------------------------------------
CUTKEE START
    MAXGRA  200
    MAXNDL  1000
CUTKEE END


DYNAMIC END
"""
