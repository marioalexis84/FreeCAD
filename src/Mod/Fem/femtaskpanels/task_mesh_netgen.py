__title__ = "FreeCAD FEM mesh netgen task panel for the document object"
__author__ = "Mario Passaglia"
__url__ = "https://www.freecad.org"

## @package task_mesh_netgen
#  \ingroup FEM
#  \brief task panel for mesh netgen object

import sys
import time

from PySide import QtCore
from PySide import QtGui
from PySide.QtCore import Qt
from PySide.QtGui import QApplication

import FreeCAD
import FreeCADGui

import FemGui
from femtools.femutils import is_of_type
from femtools.femutils import getOutputWinColor
from . import base_femtaskpanel


class _TaskPanel(base_femtaskpanel._BaseTaskPanel):
    """
    The TaskPanel for editing References property of
    MeshNetgen objects and creation of new FEM mesh
    """

    def __init__(self, obj):
        super().__init__(obj)

        self.form = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/Fem/Resources/ui/MeshNetgen.ui"
        )
        self.Timer = QtCore.QTimer()
        self.Timer.start(100)  # 100 milli seconds
        self.netgen_runs = False
        self.console_message_netgen = ""

        QtCore.QObject.connect(
            self.form.qsb_max_size,
            QtCore.SIGNAL("valueChanged(Base::Quantity)"),
            self.max_size_changed,
        )
        QtCore.QObject.connect(
            self.form.qsb_min_size,
            QtCore.SIGNAL("valueChanged(Base::Quantity)"),
            self.min_size_changed,
        )
        QtCore.QObject.connect(
            self.form.dsb_seg_per_edge,
            QtCore.SIGNAL("valueChanged(double)"),
            self.seg_per_edge_changed,
        )
        QtCore.QObject.connect(
            self.form.dsb_curvature_safety,
            QtCore.SIGNAL("valueChanged(double)"),
            self.curvature_safety_changed,
        )
        QtCore.QObject.connect(
            self.form.dsb_growth_rate,
            QtCore.SIGNAL("valueChanged(double)"),
            self.growth_rate_changed,
        )
        QtCore.QObject.connect(
            self.form.ckb_second_order,
            QtCore.SIGNAL("toggled(bool)"),
            self.second_order_changed
        )
        QtCore.QObject.connect(
            self.form.cb_fineness,
            QtCore.SIGNAL("currentIndexChanged(int)"),
            self.fineness_changed,
        )
        QtCore.QObject.connect(self.Timer,
            QtCore.SIGNAL("timeout()"),
            self.update_timer_text)
        QtCore.QObject.connect(
            self.form.pb_get_netgen_version,
            QtCore.SIGNAL("clicked()"),
            self.get_netgen_version
        )

        self.get_mesh_params()
        self.set_widgets()

    def getStandardButtons(self):
        button_value = (
            QtGui.QDialogButtonBox.Ok | QtGui.QDialogButtonBox.Apply | QtGui.QDialogButtonBox.Cancel
        )
        return button_value
        # show a OK, a apply and a Cancel button
        # def reject() is called on Cancel button
        # def clicked(self, button) is needed, to access the apply button

    def accept(self):
        self.set_mesh_params()
        return super().accept()

    def reject(self):
        self.Timer.stop()
        return super().reject()

    def clicked(self, button):
        if button == QtGui.QDialogButtonBox.Apply:
            self.set_mesh_params()
            self.run_netgen()

    def get_mesh_params(self):
        self.min_size = self.obj.MinSize
        self.max_size = self.obj.MaxSize
        self.fineness = self.obj.Fineness
        print("fineness: ", self.fineness)
        self.growth_rate = self.obj.GrowthRate
        self.curvature_safety = self.obj.CurvatureSafety
        self.seg_per_edge = self.obj.SegmentsPerEdge
        self.second_order = self.obj.SecondOrder

    def set_mesh_params(self):
        self.obj.MinSize = self.min_size
        self.obj.MaxSize = self.max_size
        self.obj.Fineness = self.fineness
        self.obj.GrowthRate = self.growth_rate
        self.obj.CurvatureSafety = self.curvature_safety
        self.obj.SegmentsPerEdge = self.seg_per_edge
        self.obj.SecondOrder = self.second_order

    def set_widgets(self):
        "fills the widgets"
        self.form.qsb_max_size.setProperty("value", self.max_size)
        FreeCADGui.ExpressionBinding(self.form.qsb_max_size).bind(self.obj, "MaxSize")

        self.form.qsb_min_size.setProperty("value", self.min_size)
        FreeCADGui.ExpressionBinding(self.form.qsb_min_size).bind(self.obj, "MinSize")

        self.fineness_enum = self.obj.getEnumerationsOfProperty("Fineness")
        index = self.fineness_enum.index(self.fineness)
        print("index0: ", index)
        self.form.cb_fineness.addItems(self.fineness_enum)
        self.form.cb_fineness.setCurrentIndex(index)
        print(self.fineness_enum, self.fineness)
        print("index: ", index)
        self.form.dsb_growth_rate.setValue(self.growth_rate)
        self.form.dsb_curvature_safety.setValue(self.curvature_safety)
        self.form.dsb_seg_per_edge.setValue(self.seg_per_edge)

        self.form.ckb_second_order.setChecked(self.second_order)

    def console_log(self, message="", outputwin_color_type=None):
        self.console_message_netgen = self.console_message_netgen + (
            '<font color="{}"><b>{:4.1f}:</b></font> '.format(
                getOutputWinColor("Logging"), time.time() - self.Start
            )
        )
        if outputwin_color_type:
            if outputwin_color_type == "#00AA00":  # Success is not part of output window parameters
                self.console_message_netgen += '<font color="{}">{}</font><br>'.format(
                    outputwin_color_type, message
                )
            else:
                self.console_message_netgen += '<font color="{}">{}</font><br>'.format(
                    getOutputWinColor(outputwin_color_type), message
                )
        else:
            self.console_message_netgen += message + "<br>"
        self.form.te_output.setText(self.console_message_netgen)
        self.form.te_output.moveCursor(QtGui.QTextCursor.End)

    def update_timer_text(self):
        # FreeCAD.Console.PrintMessage("timer1\n")
        if self.netgen_runs:
#            FreeCAD.Console.PrintMessage("timer2\n")
            # FreeCAD.Console.PrintMessage("Time: {0:4.1f}: \n".format(time.time() - self.Start))
            self.form.l_time.setText(f"Time: {time.time() - self.Start:4.1f}: ")

    def max_size_changed(self, base_quantity_value):
        self.max_size = base_quantity_value

    def min_size_changed(self, base_quantity_value):
        self.min_size = base_quantity_value

    def seg_per_edge_changed(self, value):
        self.seg_per_edge = value

    def curvature_safety_changed(self, value):
        self.curvature_safety = value

    def growth_rate_changed(self, value):
        self.growth_rate = value

    def fineness_changed(self, index):
        print("index changed: ", index)
        self.fineness = self.fineness_enum[index]
        if self.fineness == "UserDefined":
            self.form.dsb_seg_per_edge.setEnabled(True)
            self.form.dsb_growth_rate.setEnabled(True)
            self.form.dsb_curvature_safety.setEnabled(True)
        else:
            self.form.dsb_seg_per_edge.setEnabled(False)
            self.form.dsb_growth_rate.setEnabled(False)
            self.form.dsb_curvature_safety.setEnabled(False)

    def second_order_changed(self, bool_value):
        self.second_order = bool_value

    def get_netgen_version(self):
        from femmesh import netgentools

        full_message = netgentools.netgen_version()
        QtGui.QMessageBox.information(None, "Netgen - Information", full_message)

    def run_netgen(self):
        from femmesh import netgentools

        netgen_mesh = netgentools.NetgenTools(self.obj)
        thr = netgentools.T(netgen_mesh)
        #        part = self.obj.Shape
        #            netgen_mesh.outputCompoundWarning()
        self.Start = time.time()
        self.form.l_time.setText(f"Time: {time.time() - self.Start:4.1f}: ")
        #        self.console_message_netgen = ""
        self.netgen_runs = True
        self.console_log("We are going to start ...")
        #        self.get_active_analysis()
        self.console_log("Start Netgen ...")
#        QApplication.setOverrideCursor(Qt.WaitCursor)
        try:
#            error = netgen_mesh.create_mesh()
            error = thr.start()
        except Exception:
            error = sys.exc_info()[1]
            FreeCAD.Console.PrintError(f"Unexpected error when creating mesh: {error}\n")
#        if error:
#            FreeCAD.Console.PrintWarning("Netgen had warnings:\n")
#            FreeCAD.Console.PrintWarning(f"{error}\n")
#            self.console_log("Netgen had warnings ...", "Warning")
#            self.console_log(error, "Error")
#        else:
#            FreeCAD.Console.PrintMessage("Clean run of Netgen\n")
#            self.console_log("Clean run of Netgen", "#00AA00")
#        self.console_log("Netgen done!")
        self.form.l_time.setText(f"Time: {time.time() - self.Start:4.1f}: ")
        self.Timer.stop()
        #        self.update()
#        QApplication.restoreOverrideCursor()

    def get_active_analysis(self):
        analysis = FemGui.getActiveAnalysis()
        if not analysis:
            FreeCAD.Console.PrintLog("No active analysis, means no group meshing.\n")
            self.analysis = None  # no group meshing
        else:
            for m in analysis.Group:
                if m.Name == self.obj.Name:
                    FreeCAD.Console.PrintLog(f"Active analysis found: {analysis.Name}\n")
                    self.analysis = analysis  # group meshing
                    break
            else:
                FreeCAD.Console.PrintLog(
                    "Mesh is not member of active analysis, means no group meshing.\n"
                )
                self.analysis = None  # no group meshing
        return
