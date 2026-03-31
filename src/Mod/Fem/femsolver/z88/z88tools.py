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

__title__ = "Tools for the work with Z88 solver"
__author__ = "Mario Passaglia"
__url__ = "https://www.freecad.org"


from PySide.QtCore import QProcess, QProcessEnvironment
import os
import re

import numpy as np

from vtkmodules.util import numpy_support as vtk_np
from vtkmodules.vtkCommonDataModel import vtkCellArray, vtkUnstructuredGrid
from vtkmodules.vtkCommonCore import vtkPoints

import FreeCAD

from . import writer
from .. import settings
from . import z88utils

from femmesh import meshsetsgetter
from femtools import membertools
from femtools.objecttools import ObjectTools


class Z88Tools(ObjectTools):

    name = "Z88"

    def __init__(self, obj):
        super().__init__(obj)
        self.model_file = ""

    def prepare(self):
        self._clear_results()

        mesh_obj = membertools.get_mesh_to_solve(self.analysis)[0]
        meshdatagetter = meshsetsgetter.MeshSetsGetter(
            self.analysis,
            self.obj,
            mesh_obj,
            membertools.AnalysisMember(self.analysis),
        )

        # set masks
        masks = z88utils.define_masks()
        meshdatagetter.mask_tria6 = masks["tria6"]
        meshdatagetter.mask_quad8 = masks["quad8"]
        meshdatagetter.mask_tetra4 = masks["tetra4"]
        meshdatagetter.mask_tetra10 = masks["tetra10"]
        meshdatagetter.mask_hexa8 = masks["hexa8"]
        meshdatagetter.mask_hexa20 = masks["hexa20"]

        meshdatagetter.get_mesh_sets()

        # write solver input
        w = writer.FemInputWriterZ88(
            self.analysis,
            self.obj,
            mesh_obj,
            meshdatagetter.member,
            self.obj.WorkingDirectory,
        )
        w.write_solver_input()

    def compute(self):
        self._clear_results()
        z88_bin = settings.get_binary("Z88")
        env = QProcessEnvironment.systemEnvironment()
        self.process.setProcessEnvironment(env)
        self.process.setWorkingDirectory(self.obj.WorkingDirectory)
        command_list = ["-t", "-" + self.obj.SolverType]

        if self.obj.AnalysisType == "test":
            self.process.start(z88_bin, command_list)
        else:
            # first run test mode (mandatory) in another process to prevent self.process
            # emit signals
            prepare_process = QProcess()
            prepare_env = QProcessEnvironment.systemEnvironment()
            prepare_process.setProcessEnvironment(prepare_env)
            prepare_process.setWorkingDirectory(self.obj.WorkingDirectory)
            prepare_process.start(z88_bin, command_list)
            prepare_process.waitForFinished(-1)

            # compute mode
            command_list = ["-c", "-" + self.obj.SolverType]
            self.process.start(z88_bin, command_list)

        return self.process

    def update_properties(self):
        self._load_vtk_results()
        self._load_dat_results()

    def _clear_results(self):
        # results are z88oN.txt files
        dir_content = os.listdir(self.obj.WorkingDirectory)
        for f in dir_content:
            if re.match(r"^z88o\d+\.txt$", f):
                path = os.path.join(self.obj.WorkingDirectory, f)
                os.remove(path)

    def _load_dat_results(self):
        # search dat output
        keep_result = self.fem_param.GetGroup("General").GetBool("KeepResultsOnReRun", False)
        dat = None
        for res in self.obj.Results:
            if res.isDerivedFrom("App::TextDocument"):
                dat = res

        if not dat or keep_result:
            # create dat output
            dat = self.obj.Document.addObject("App::TextDocument", self.obj.Name + "Output")
            self.analysis.addObject(dat)
            tmp = self.obj.Results
            tmp.append(dat)
            self.obj.Results = tmp

        files = os.listdir(self.obj.WorkingDirectory)
        # load stress results. If test mode, load log file
        target_file = self.log_file if self.obj.AnalysisType == "test" else self.stress_file
        for f in files:
            if f.lower() == target_file:
                dat_file = os.path.join(self.obj.WorkingDirectory, f)
                with open(dat_file, "r") as file:
                    dat.Text = file.read()
                break

    def _load_vtk_results(self):
        # search current pipeline
        keep_result = self.fem_param.GetGroup("General").GetBool("KeepResultsOnReRun", False)
        pipeline = None
        create = False
        for res in self.obj.Results:
            if res.isDerivedFrom("Fem::FemPostPipeline"):
                pipeline = res

        if not pipeline or keep_result:
            # create pipeline
            pipeline = self.obj.Document.addObject("Fem::FemPostPipeline", self.obj.Name + "Result")
            self.analysis.addObject(pipeline)
            tmp = self.obj.Results
            tmp.append(pipeline)
            self.obj.Results = tmp
            create = True

        grid = self.generate_mesh()
        pipeline.Data = grid
        if self.obj.DisplaceMesh and self.obj.AnalysisType != "test":
            self._generate_disp_mesh(pipeline)

        if create and FreeCAD.GuiUp:
            # default display mode
            view_obj = pipeline.ViewObject
            view_obj.DisplayMode = "Surface"
            view_obj.SelectionStyle = "BoundBox"
            enum_field = view_obj.getEnumerationsOfProperty("Field")
            default_field = self._get_default_field()
            if default_field in enum_field:
                view_obj.Field = default_field

    def _get_default_field(self):
        return "Displacement"

    def _generate_disp_mesh(self, pipeline):
        try:
            grid = pipeline.Data
            if not grid.GetPointData().HasArray("Displacement"):
                return
            points = grid.GetPoints()
            pd = points.GetData()
            disp = grid.GetPointData().GetAbstractArray("Displacement")
            # check 2d analysis
            disp = vtk_np.vtk_to_numpy(disp)
            disp = disp.reshape((len(disp), -1))
            rows, cols = disp.shape
            if cols == 2:
                # plane stress, axisymmetric models
                disp = np.hstack((disp, np.zeros((rows, 1))))
            elif cols == 1:
                # plate model
                disp = np.hstack((np.zeros((rows, 2)), disp))

            disp_points = vtk_np.vtk_to_numpy(pd) + disp
            disp_points = vtk_np.numpy_to_vtk(disp_points)
            points.SetData(disp_points)

            pipeline.Data = grid
        except Exception as e:
            raise e

    def generate_mesh(self):
        file_o0 = open(os.path.join(self.obj.WorkingDirectory, self.mesh_file), "r")
        struct_data = None
        grid = vtkUnstructuredGrid()
        # search structure data
        for line in file_o0:
            m = re.search(r"\w+\=(?P<dof>\d+)\s*\w+=(?P<nodes>\d+)\s*\w+=(?P<elements>\d+)", line)
            if m:
                struct_data = m.groupdict()
                break
        if struct_data is None:
            FreeCAD.Console.PrintError("No Z88 mesh data")
            return grid

        # search nodes begin
        for line in file_o0:
            m = re.search(r"X\s+Y\s+Z?$", line)
            if m:
                break

        # read nodes
        nodes = np.loadtxt(file_o0, max_rows=int(struct_data["nodes"]))

        # search element begin
        for line in file_o0:
            m = re.search(r"(\s+i\d+){20}", line)
            if m:
                break

        dt = np.dtype(
            {
                "names": ["nodes", "elem", "type", "size"],
                "formats": [("i8", (20,)), "i4", "i8", "i8"],
            }
        )
        elements = np.zeros([int(struct_data["elements"])], dtype=dt)

        for i in range(int(struct_data["elements"])):
            # get element number and type
            el, tp = file_o0.readline().split()
            # get element nodes
            e_n = file_o0.readline().split()
            # skip emtpy line
            file_o0.readline()
            # fill structured array
            zero_row = np.zeros([20])

            zero_row[: len(e_n)] = e_n
            elements[i] = (zero_row, el, tp, len(e_n))

        # nodes order and element type conversion (override z88 data)
        for el in elements:
            vtk_order = el["nodes"][: el["size"]][self.z88_to_vtk_order[el["type"]]]
            el["nodes"][: el["size"]] = vtk_order
            el["type"] = self.z88_to_vtk_type[el["type"]]

        # connectivity (subtract 1. Z88 nodes start from 1)
        connectivity = (elements["nodes"][elements["nodes"] != 0]).reshape(-1) - 1
        # offsets
        offsets = np.insert(np.add.accumulate(elements["size"]), 0, 0)

        vtk_connectivity = vtk_np.numpy_to_vtkIdTypeArray(connectivity, deep=True)
        vtk_offsets = vtk_np.numpy_to_vtkIdTypeArray(offsets, deep=True)

        cell_array = vtkCellArray()
        cell_array.SetData(vtk_offsets, vtk_connectivity)

        points = vtkPoints()
        points.SetData(vtk_np.numpy_to_vtk(nodes[:, 2:]))
        grid.SetPoints(points)
        grid.SetCells(elements["type"].tolist(), cell_array)
        if self.obj.AnalysisType != "test":
            self.generate_result(grid)
            self.load_force(grid)

        return grid

    def generate_result(self, grid):
        file_o2 = open(os.path.join(self.obj.WorkingDirectory, self.disp_file), "r")
        # search element begin
        for line in file_o2:
            m = re.search(r"(\s+U\(\d\)){6}", line)
            if m:
                break

        # read values
        disp = np.loadtxt(file_o2)
        # plate model disp_x -> disp_z, disp_y -> rot_x, disp_z -> rot_y
        if self.obj.ModelSpace == "plate":
            disp_vtk = vtk_np.numpy_to_vtk(disp[:, 1:2], deep=True)
            disp_vtk.SetName("Displacement")
            grid.GetPointData().AddArray(disp_vtk)
            # set rotations in degrees here until we have a general method to set units
            disp_vtk = vtk_np.numpy_to_vtk(disp[:, 2:4] * 180 / np.pi, deep=True)
            disp_vtk.SetName("Rotation")
            grid.GetPointData().AddArray(disp_vtk)
        else:
            disp_vtk = vtk_np.numpy_to_vtk(disp[:, 1:4], deep=True)
            disp_vtk.SetName("Displacement")
            grid.GetPointData().AddArray(disp_vtk)

    def load_force(self, grid):
        file_o4 = open(os.path.join(self.obj.WorkingDirectory, self.force_file), "r")
        # read values
        forces = file_o4.readlines()
        n_nodes = grid.GetPoints().GetNumberOfPoints()
        # only total force at nodes
        forces = forces[-n_nodes:]
        forces = np.loadtxt(forces)

        # plate model force_x -> force_z, force_y -> moment_x, force_z -> moment_y
        if self.obj.ModelSpace == "plate":
            forces_vtk = vtk_np.numpy_to_vtk(forces[:, 1:2], deep=True)
            forces_vtk.SetName("Nodal Force")
            grid.GetPointData().AddArray(forces_vtk)
            moments_vtk = vtk_np.numpy_to_vtk(forces[:, 2:4], deep=True)
            moments_vtk.SetName("Nodal Moment")
            grid.GetPointData().AddArray(moments_vtk)
        else:
            forces_vtk = vtk_np.numpy_to_vtk(forces[:, 1:4], deep=True)
            forces_vtk.SetName("Nodal Force")
            grid.GetPointData().AddArray(forces_vtk)
            moments_vtk = vtk_np.numpy_to_vtk(forces[:, 4:], deep=True)
            moments_vtk.SetName("Nodal Moment")
            grid.GetPointData().AddArray(moments_vtk)

    def version(self):
        return "Version: ?"

    mesh_file = "z88o0.txt"
    disp_file = "z88o2.txt"
    stress_file = "z88o3.txt"
    force_file = "z88o4.txt"
    log_file = "z88r.log"

    # z88 to vtk element order
    z88_to_vtk_order = {
        1: [0, 4, 5, 1, 3, 7, 6, 2],  # hexa8
        2: [0, 1],  # beam2
        3: [0, 1, 2, 3, 4, 5],  # tria6
        4: [0, 1],  # truss2 space
        5: [0, 1],  # shaft2
        6: [0, 1, 2],  # torus3
        7: [0, 1, 2, 3, 4, 5, 6, 7],  # ps8
        8: [0, 1, 2, 3, 4, 5, 6, 7],  # torus8
        9: [0, 1],  # truss2 plane
        10: [6, 7, 4, 5, 2, 3, 0, 1, 14, 15, 12, 13, 10, 11, 8, 9, 18, 19, 16, 17],  # hexa20
        13: [0, 1],  # beam2 plane
        14: [0, 1, 2, 3, 4, 5],  # ps6
        15: [0, 1, 2, 3, 4, 5],  # torus6
        16: [0, 1, 2, 3, 4, 5, 6, 9, 7, 8],  # tetra10
        17: [0, 1, 2, 3],  # tetra4
        18: [0, 1, 2, 3, 4, 5],  # plate6
        20: [0, 1, 2, 3, 4, 5, 6, 7],  # plate8
        23: [0, 1, 2, 3, 4, 5, 6, 7],  # shell8
        24: [0, 1, 2, 3, 4, 5],  # shell6
        25: [0, 1],  # beam2
    }

    # z88 to vtk element type
    z88_to_vtk_type = {
        1: vtk_np.vtkConstants.VTK_HEXAHEDRON,
        2: vtk_np.vtkConstants.VTK_LINE,
        3: vtk_np.vtkConstants.VTK_QUADRATIC_TRIANGLE,
        4: vtk_np.vtkConstants.VTK_LINE,
        5: vtk_np.vtkConstants.VTK_LINE,
        6: vtk_np.vtkConstants.VTK_TRIANGLE,
        7: vtk_np.vtkConstants.VTK_QUADRATIC_QUAD,
        8: vtk_np.vtkConstants.VTK_QUADRATIC_QUAD,
        9: vtk_np.vtkConstants.VTK_LINE,
        10: vtk_np.vtkConstants.VTK_QUADRATIC_HEXAHEDRON,
        13: vtk_np.vtkConstants.VTK_LINE,
        14: vtk_np.vtkConstants.VTK_QUADRATIC_TRIANGLE,
        15: vtk_np.vtkConstants.VTK_QUADRATIC_TRIANGLE,
        16: vtk_np.vtkConstants.VTK_QUADRATIC_TETRA,
        17: vtk_np.vtkConstants.VTK_TETRA,
        18: vtk_np.vtkConstants.VTK_QUADRATIC_TRIANGLE,
        20: vtk_np.vtkConstants.VTK_QUADRATIC_QUAD,
        23: vtk_np.vtkConstants.VTK_QUADRATIC_QUAD,
        24: vtk_np.vtkConstants.VTK_QUADRATIC_TRIANGLE,
        25: vtk_np.vtkConstants.VTK_LINE,
    }
