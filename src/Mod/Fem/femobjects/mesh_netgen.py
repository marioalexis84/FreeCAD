__title__ = "FreeCAD FEM mesh netgen document object"
__author__ = "Mario Passaglia"
__url__ = "https://www.freecad.org"

## @package mesh_netgen
#  \ingroup FEM
#  \brief mesh gmsh object

from FreeCAD import Base
from . import base_fempythonobject

_PropHelper = base_fempythonobject._PropHelper


class MeshNetgen(base_fempythonobject.BaseFemPythonObject):
    """
    A Fem::FemMeshShapeBaseObject python type, add Netgen specific properties
    """

    Type = "Fem::FemMeshNetgen"

    def __init__(self, obj):
        super().__init__(obj)

        for prop in self._get_properties():
            prop.add_to_object(obj)

    def _get_properties(self):
        prop = []
# opterrpow = 2
# blockfill = 1
# filldist = 0.1
# safety = 5
# relinnersafety = 3
# uselocalh = 1
# delaunay = 1
# meshsizefilename = 
# startinsurface = 0
# checkoverlap = 1
# checkchartboundary = 1
# parthread = 0
# giveuptol2d = 200
# giveuptol = 10
# maxoutersteps = 10
# starshapeclass = 5
# baseelnp        = 0
# sloppy = 1
# badellimit = 175
# inverttets = 0
# inverttrigs = 0


        prop.append(
            _PropHelper(
                type="App::PropertyString",
                name="Optimize3d",
                group="Mesh Parameters",
                doc="3d optimization strategy.\n"
                + "m: move nodes, M: move nodes, cheap functional\n"
                + "s: swap faces, c: combine elements, d: divide elements,\n"
                + "D: divide and join opposite edges, remove element,\n"
                + "p: plot, no pause, P: plot, Pause,\n"
                + "h: Histogramm, no pause, H: Histogramm, pause",
                value="cmdDmustm",
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyEnumeration",
                name="Fineness",
                group="Mesh Parameters",
                doc="Fineness",
                value=["VeryCoarse", "Coarse", "Moderate", "Fine", "VeryFine", "UserDefined"],
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="OptimizationSteps3d",
                group="Mesh Parameters",
                doc="Number of 3d optimization steps",
                value=3,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyString",
                name="Optimize2d",
                group="Mesh Parameters",
                doc="2d optimization strategy.\n"
                + "s: swap opt 6 lines/node, S: swap optimal elements,\n"
                + "m: move nodes, p: plot, no pause\n"
                + "P: plot, pause, c: combine",
                value="smcmSmcmSmcm",
            )
        )

        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="OptimizationSteps2d",
                group="Mesh Parameters",
                doc="Number of 2d optimization steps",
                value=3,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyFloat",
                name="OptimizationErrorPower",
                group="Mesh Parameters",
                doc="Power of error to approximate max error optimization",
                value=2.0,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="BlockFill",
                group="Mesh Parameters",
                doc="Do block filling",
                value=True,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyLength",
                name="FillDistance",
                group="Mesh Parameters",
                doc="Block filling up to distance",
                value=0.1,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyFloat",
                name="Safety",
                group="Mesh Parameters",
                doc="Radius of local environment (times h)",
                value=5.0,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyFloat",
                name="RelinnerSafety",
                group="Mesh Parameters",
                doc="Radius of active environment (times h)",
                value=3.0,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="LocalH",
                group="Mesh Parameters",
                doc="Use local h",
                value=True,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="UseLocalH",
                group="Mesh Parameters",
                doc="Use local H",
                value=True,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyFloat",
                name="GrowthRate",
                group="Mesh Parameters",
                doc="Grading for local h",
                value=0.3,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="Delaunay",
                group="Mesh Parameters",
                doc="Use Delaunay for 3d meshing",
                value=True,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="Delaunay2d",
                group="Mesh Parameters",
                doc="Use Delaunay for 2d meshing",
                value=False,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyLength",
                name="MaxSize",
                group="Mesh Parameters",
                doc="Maximal mesh size",
                value="1000 mm",
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyLength",
                name="MinSize",
                group="Mesh Parameters",
                doc="Minimal mesh size",
                value="0 mm",
            )
        )
        #    /// file for meshsize
        #    string meshsizefilename = "";
        prop.append(
            _PropHelper(
                type="App::PropertyFloat",
                name="CloseEdgeFactor",
                group="Mesh Parameters",
                doc="Factor to restrict meshing based on close edges",
                value=2.0,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="StartInSurface",
                group="Mesh Parameters",
                doc="Start surface meshing from everywhere in surface",
                value=False,
            )
        )
        #    ///  (debug)
        #    bool checkoverlap = true;
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="CheckOverlap",
                group="Mesh Parameters",
                doc="Check overlapping surfaces",
                value=True,
            )
        )
        #    /// check overlapping surface mesh before volume meshing
        #    bool checkoverlappingboundary = true;
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="CheckOverlappingBoundary",
                group="Mesh Parameters",
                doc="Check overlapping surface mesh before volume meshing",
                value=True,
            )
        )
        #    /// check chart boundary (sometimes too restrictive)
        #    bool checkchartboundary = true;
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="CheckChartBoundary",
                group="Mesh Parameters",
                doc="Check chart boundary",
                value=True,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyFloat",
                name="CurvatureSafety",
                group="Mesh Parameters",
                doc="Safety factor for curvatures (elements per radius)",
                value=2.0,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyFloat",
                name="SegmentsPerEdge",
                group="Mesh Parameters",
                doc="Minimal number of segments per edge",
                value=2.0,
            )
        )
        ##    /// use parallel threads
        ##    bool parthread = 0;
        prop.append(
            _PropHelper(
                type="App::PropertyFloat",
                name="ElementSizeWeight",
                group="Mesh Parameters",
                doc="Weight of element size respect to  element shape",
                value=0.2,
            )
        )
        ##    /// init with default values
        ##
        ##    /// start at step
        ##    int perfstepsstart = 0;
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="StartStep",
                group="Mesh Parameters",
                doc="Start at step",
                value=0,
            )
        )
        ##    /// end at step
        ##    int perfstepsend = 6;
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="EndStep",
                group="Mesh Parameters",
                doc="EndStep",
                value=6,
            )
        )
        ##    /// give up quality class, 2d meshing
        ##    int giveuptol2d = 200;
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="GiveUpTolerance2d",
                group="Mesh Parameters",
                doc="Give up quality class, 2d meshing",
                value=200,
            )
        )
        ##    /// give up quality class, 3d meshing
        ##    int giveuptol = 10;
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="GiveUpTolerance",
                group="Mesh Parameters",
                doc="Give up quality class, 3d meshing",
                value=10,
            )
        )
        ##    /// give up quality class for closing open quads, > 100 for
        ##    /// free pyramids
        ##    int giveuptolopenquads = 15;
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="GiveUpToleranceOpenQuads",
                group="Mesh Parameters",
                doc="Give up quality class, for closing open quads, greather than 100 for free pyramids",
                value=15,
            )
        )
        ##    /// maximal outer steps
        ##    int maxoutersteps = 10;
        ##    /// class starting star-shape filling
        ##    int starshapeclass = 5;
        ##    /// if non-zero, baseelement must have baseelnp points
        ##    int baseelnp = 0;
        ##    /// quality tolerances are handled less careful
        ##    int sloppy = 1;
        ##
        ##    /// limit for max element angle (150-180)
        ##    double badellimit = 175;
        ##
        ##    bool check_impossible = false;
        ##
        ##    int only3D_domain_nr = 0;
        ##
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="SecondOrder",
                group="Mesh Parameters",
                doc="Second order element meshing",
                value=True,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="ElementOrder",
                group="Mesh Parameters",
                doc="High order element curvature",
                value=False,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="QuadDominated",
                group="Mesh Parameters",
                doc="Quad-dominated surface meshing",
                value=False,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="TryHexes",
                group="Mesh Parameters",
                doc="Try hexahedral elements",
                value=False,
            )
        )
        ##    ///
        ##    bool inverttets = false;
        ##    ///
        ##    bool inverttrigs = false;

        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="AutoZRefine",
                group="Mesh Parameters",
                doc="Automatic Z refine",
                value=False,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="ParallelMeshing",
                group="Mesh Parameters",
                doc="Use parallel meshing",
                value=True,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyInteger",
                name="Threads",
                group="Mesh Parameters",
                doc="Number of threads for parallel meshing",
                value=4,
            )
        )
        prop.append(
            _PropHelper(
                type="App::PropertyBool",
                name="HealShape",
                group="Mesh Parameters",
                doc="Heal shape before meshing",
                value=True,
            )
        )

        return prop
