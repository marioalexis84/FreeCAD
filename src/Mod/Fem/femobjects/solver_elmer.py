# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2017 Markus Hovorka <m.hovorka@live.de>                 *
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

__title__ = "FreeCAD FEM solver Elmer"
__author__ = "Markus Hovorka, Mario Passaglia"
__url__ = "https://www.freecad.org"

## @package solver_elmer
#  \ingroup FEM
#  \brief solver Elmer object

from . import base_fempythonobject

_PropHelper = base_fempythonobject._PropHelper


class SolverElmer(base_fempythonobject.BaseFemPythonObject):

    Type = "Fem::SolverElmer"

    def __init__(self, obj):
        super().__init__(obj)
        obj.addExtension("App::GroupExtensionPython")

        for prop in self._get_properties():
            prop.add_to_object(obj)

    def _get_properties(self):
        prop = []

        prop.append(
            _PropHelper(
                type="App::PropertyEnumeration",
                name="CoordinateSystem",
                group="Coordinate System",
                doc="Coordinate system to use",
                value=[
                    "Cartesian",
                    "Cartesian 1D",
                    "Cartesian 2D",
                    "Cartesian 3D",
                    "Polar 2D",
                    "Polar 3D",
                    "Cylindric",
                    "Cylindric Symmetric",
                    "Axi Symmetric",
                ],
            )
        )

        #        obj.addProperty("App::PropertyEnumeration", "CoordinateSystem", "Coordinate System", "")
        #        obj.CoordinateSystem = COORDINATE_SYSTEM
        #        obj.CoordinateSystem = "Cartesian"

        prop.append(
            _PropHelper(
                type="App::PropertyIntegerConstraint",
                name="BDFOrder",
                group="Timestepping",
                doc="Order of time stepping method 'BDF'",
                value={"value": 2, "min": 1, "max": 5, "step": 1},
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyIntegerList",
                name="OutputIntervals",
                group="Timestepping",
                doc="After how many time steps a result file is output",
                value=[1],
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyIntegerList",
                name="TimestepIntervals",
                group="Timestepping",
                doc="List of times if 'SimulationType' is 'Scanning' or 'Transient'",
                value=[100],
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyFloatList",
                name="TimestepSizes",
                group="Timestepping",
                doc="List of time steps if 'SimulationType' is 'Scanning' or 'Transient'",
                value=[0.1],
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyEnumeration",
                name="SimulationType",
                group="Type",
                doc="",
                value=["Scanning", "Steady State", "Transient"],
            )
        )

        # obj.SimulationTyvalue=pe = "Steady State"

        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="SteadyStateMaxIterations",
                group="Type",
                doc="Maximal steady state iterations",
                value=1,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="SteadyStateMinIterations",
                group="Type",
                doc="Minimal steady state iterations",
                value=0,
            )
        )

        return prop


#        obj.addProperty("App::PropertyLink", "ElmerResult", "Base", "", 4 | 8)

#        obj.addProperty("App::PropertyLink", "ElmerOutput", "Base", "", 4 | 8)

#        obj.addProperty(
#            "App::PropertyBool", "BinaryOutput", "Result File", "Save result in binary format"
#        )
#        obj.BinaryOutput = False
#
#        obj.addProperty(
#            "App::PropertyBool", "SaveGeometryIndex", "Result File", "Save geometry IDs"
#        )
#        obj.SaveGeometryIndex = False

#    def onDocumentRestored(self, obj):
# update old project with new properties
#        try:
#            obj.getPropertyByName("BinaryOutput")
#        except FreeCAD.Base.PropertyError:
#            obj.addProperty(
#                "App::PropertyBool", "BinaryOutput", "Result File", "Save result in binary format"
#            )
#            obj.BinaryOutput = False
#        try:
#            obj.getPropertyByName("SaveGeometryIndex")
#        except FreeCAD.Base.PropertyError:
#            obj.addProperty(
#                "App::PropertyBool", "SaveGeometryIndex", "Result File", "Save geometry IDs"
#            )
#            obj.SaveGeometryIndex = False

##    def createMachine(self, obj, directory, testmode=False):
##        return run.Machine(
##            solver=obj,
##            directory=directory,
##            check=tasks.Check(),
##            prepare=tasks.Prepare(),
##            solve=tasks.Solve(),
##            results=tasks.Results(),
##            testmode=testmode,
##        )
##
##    def createEquation(self, doc, eqId):
##        return self._EQUATIONS[eqId].create(doc)
##
##    def isSupported(self, eqId):
##        return eqId in self._EQUATIONS
##
##    def editSupported(self):
##        return True
##
##    def edit(self, directory):
##        pattern = os.path.join(directory, "case.sif")
##        FreeCAD.Console.PrintMessage(f"{pattern}\n")
##        f = glob.glob(pattern)[0]
##        FemGui.open(f)
##
##
##class ViewProxy(solverbase.ViewProxy):
##    """Proxy for FemSolverElmers View Provider."""
##
##    def getIcon(self):
##        return ":/icons/FEM_SolverElmer.svg"
##
##
##  @}
