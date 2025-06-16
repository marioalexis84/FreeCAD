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

__title__ = "Base class for the work with solvers and meshers"
__author__ = "Mario Passaglia"
__url__ = "https://www.freecad.org"

from PySide import QtCore
from abc import ABC, abstractmethod
import tempfile
import os

import FreeCAD


class BaseObjectTools(ABC):
    def __init__(self, obj):
        self.obj = obj
        self.process = QtCore.QProcess()
        self.analysis = obj.getParentGroup()
        self.fem_param = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Fem")
        self._create_working_directory(obj)
        QtCore.QObject.connect(
            self.process,
            QtCore.SIGNAL("finished(int, QProcess::ExitStatus)"),
            self._process_finished,
        )

    def _create_working_directory(self, obj):
        """
        Create working directory according to preferences
        """
        if not os.path.isdir(obj.WorkingDirectory):
            gen_param = self.fem_param.GetGroup("General")
            if gen_param.GetBool("UseTempDirectory"):
                self.obj.WorkingDirectory = tempfile.mkdtemp(prefix="fem_")
            elif gen_param.GetBool("UseBesideDirectory"):
                root, ext = os.path.splitext(obj.Document.FileName)
                if root:
                    self.obj.WorkingDirectory = os.path.join(root, obj.Label)
                    os.makedirs(self.obj.WorkingDirectory, exist_ok=True)
                else:
                    # file not saved, use temporary
                    self.obj.WorkingDirectory = tempfile.mkdtemp(prefix="fem_")
            elif gen_param.GetBool("UseCustomDirectory"):
                self.obj.WorkingDirectory = gen_param.GetString("CustomDirectoryPath")
                os.makedirs(self.obj.WorkingDirectory, exist_ok=True)

    @abstractmethod
    def prepare(self):
        pass

    @abstractmethod
    def compute(self):
        pass

    @abstractmethod
    def update_properties(self):
        """
        Update FreeCAD feature properties
        """
        pass

    def run(self):
        """
        Prepare and compute
        """
        print("INICIA PREPARACION")
        self.prepare()
        print("INICIA COMPUTO")
        self.compute()
        print("FIN COMPUTO")

    def _process_finished(self, code, status):
        print("TERMINA", self)
        if status == QtCore.QProcess.ExitStatus.NormalExit and code == 0:
            print("UPDATE:", self)
            self.update_properties()
