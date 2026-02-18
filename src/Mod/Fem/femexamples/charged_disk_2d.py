# to run the example use:
"""
from femexamples.charged_disk_2d import setup
setup()

"""


import sys
import FreeCAD

import Fem
import Part
import ObjectsFem
from Materials import MaterialManager
from . import manager


def get_information():
    return {
        "name": "Charged disk",
        "meshtype": "face",
        "meshelement": "Tria6",
        "constraints": ["electrostatic potential", "electric charge density"],
        "solvers": ["elmer", "calculix"],
        "material": "multimaterial",
        "equations": [],
    }


def get_explanation(header=""):
    return (
        header
        + """

To run the example from Python console use:
from femexamples.charged_disk_2d import setup
setup()

Electrostatic system - Elmer solver, CalculiX solver

Cylindrical region with charge density \rho surrounded by
polycarbonate and air with outer surface grounded.
      _                      _
     /  rho*a^2/(2*e_1)*(-1/2*(r/a)^2 + 1/2 + log(b/a) + e_1/e_2*log(c/b)) if r < a
    |
V = |   rho*a^2/(2*e_1)*(log(b/r) + e_1/e_2*log(c/b)) if a < r <= b
    |
     \_ rho*a^2/(2*e_2)*log(c/r) if b < r <= c

"""
    )


def setup(doc=None, solvertype="calculix"):

    # init FreeCAD document
    if doc is None:
        doc = manager.init_doc()

    # explanation object
    # just keep the following line and change text string in get_explanation method
    manager.add_explanation_obj(doc, get_explanation(manager.get_header(get_information())))

    # geometric objects

    circ_50 = Part.makeCircle(5)
    circ_25 = Part.makeCircle(2.5)
    circ_5 = Part.makeCircle(0.5)

    sector_int = Part.makeFace([circ_5])
    sector_mid = Part.makeFace([circ_5, circ_25])
    sector_ext = Part.makeFace([circ_25, circ_50])

    Shell = doc.addObject("Part::Feature", "Shell")
    Shell.Shape = Part.makeShell([sector_int, sector_mid, sector_ext])

    if FreeCAD.GuiUp:
        Shell.ViewObject.Document.activeView().viewAxonometric()
        Shell.ViewObject.Document.activeView().fitAll()
        Shell.ViewObject.Visibility = True

    # analysis
    analysis = ObjectsFem.makeAnalysis(doc, "Analysis")
    if FreeCAD.GuiUp:
        import FemGui

        FemGui.setActiveAnalysis(analysis)

    # solver
    if solvertype == "elmer":
        solver_obj = ObjectsFem.makeSolverElmer(doc, "SolverElmer")
        solver_obj.SimulationType = "Steady State"
        eq_electrostatic = ObjectsFem.makeEquationElectrostatic(doc, solver_obj)
        eq_electrostatic.CalculateElectricField = True
        eq_electrostatic.CalculateElectricFlux = True
        eq_electrostatic.CalculateCapacitanceMatrix = False
        eq_electrostatic.Priority = 1
    elif solvertype == "calculix":
        solver_obj = ObjectsFem.makeSolverCalculiX(doc, "SolverCalculiX")
        solver_obj.AnalysisType = "electromagnetic"
        solver_obj.ElectromagneticMode = "electrostatic"
        solver_obj.ModelSpace = "plane stress"
    else:
        FreeCAD.Console.PrintWarning(
            "Unknown or unsupported solver type: {}. "
            "No solver object was created.\n".format(solvertype)
        )
    analysis.addObject(solver_obj)

    # material
    material_air = ObjectsFem.makeMaterialFluid(doc, "Air")
    air_uuid = "94370b96-c97e-4a3f-83b2-11d7461f7da7"
    air = MaterialManager().getMaterial(air_uuid)
    material_air.UUID = air_uuid
    material_air.Material = air

    material_polycarbonate = ObjectsFem.makeMaterialFluid(doc, "Polycarbonate")
    polycarbonate_uuid = "2fe8c459-4ff4-4555-80a9-61d118b245c3"
    polycarbonate = dict(MaterialManager().getMaterial(polycarbonate_uuid))
    polycarbonate["RelativePermittivity"] = "2.9"
    material_polycarbonate.UUID = polycarbonate_uuid
    material_polycarbonate.Material = polycarbonate

    material_air.References = [(Shell, "Face3")]
    material_polycarbonate.References = [(Shell, ("Face1", "Face2"))]
    analysis.addObject(material_air)
    analysis.addObject(material_polycarbonate)

    # central charge
    charge = ObjectsFem.makeConstraintElectricChargeDensity(doc, "Charge")
    charge.References = [(Shell, "Face1")]
    charge.SourceChargeDensity = "1e-6 C/m^3"
    charge.Mode = "Source"
    analysis.addObject(charge)

    # boundary potential
    potential = ObjectsFem.makeConstraintElectrostaticPotential(doc, "Potential")
    potential.References = [(Shell, "Edge3")]
    potential.Potential = "0 V"
    potential.BoundaryCondition = "Dirichlet"
    analysis.addObject(potential)

    if solvertype == "calculix":
        (thickness_obj,) = analysis.addObject(
            ObjectsFem.makeElementGeometry2D(doc, name="Thickness")
        )
        thickness_obj.Thickness = "0.1 mm"
        thickness_obj.References = [(Shell, ("Face1", "Face2", "Face3"))]

    # mesh
    (femmesh_obj,) = analysis.addObject(ObjectsFem.makeMeshGmsh(doc, manager.get_meshname()))
    femmesh_obj.Shape = Shell
    femmesh_obj.ElementOrder = "2nd"
    femmesh_obj.CharacteristicLengthMax = "0.25 mm"
    femmesh_obj.ViewObject.Visibility = False
    mesh_region_obj = ObjectsFem.makeMeshRegion(doc, femmesh_obj)
    mesh_region_obj.CharacteristicLength = "0.1 mm"
    mesh_region_obj.References = [(Shell, "Face1")]

    # generate the mesh
    from femmesh import gmshtools

    gmsh_mesh = gmshtools.GmshTools(femmesh_obj, analysis)
    try:
        error = gmsh_mesh.create_mesh()
    except Exception:
        error = sys.exc_info()[1]
        FreeCAD.Console.PrintError(f"Unexpected error when creating mesh: {error}\n")

    doc.recompute()
    return doc
