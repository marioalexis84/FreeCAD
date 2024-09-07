from netgen import occ, meshing, config as ng_config
import pyngcore as ngcore
import numpy as np
import os
import time
import threading
import tempfile

import Fem


class T:
    def __init__(self, net):
        self.net = net
        self.t = threading.Thread(target=self.create)
        self.t2 = threading.Thread(target=self.g)
    def create(self):
        print("arranca mesh")
        self.net.create_mesh()
        print("finaliza mesh")

    def g(self):
        print("arranca g")
        self.t.start()
        self.t.join()
        print("finaliza g")
    def start(self):
        self.t2.start()


class NetgenTools:

    # to change order of nodes from netgen to smesh
    order_edge = {
        2 : [0, 1, 2], # seg2
        3 : [0, 1, 2], # seg3
    }
    order_face = {
        3: [0, 1, 2, 3, 4, 5, 6, 7],  # tria3
        6: [0, 1, 2, 5, 3, 4, 6, 7],  # tria6
        4: [0, 1, 2, 3, 4, 5, 6, 7],  # quad4
        8: [0, 1, 2, 3, 4, 7, 5, 6],  # quad8
    }
    order_volume = {
        4: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19],  # tetra4
        10: [0, 1, 2, 3, 4, 7, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19],  # tetra10
        8: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19],  # hexa8
        20: [0, 1, 2, 3, 4, 5, 6, 7, 8, 11, 9, 10, 12, 15, 13, 14, 16, 17, 18, 19],  # hexa20
        5: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19],  # pyra5
        13: [0, 1, 2, 3, 4, 5, 8, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19],  # pyra13
        6: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19],  # penta6
        15: [0, 1, 2, 3, 4, 5, 6, 8, 7, 12, 14, 13, 9, 10, 11, 15, 16, 17, 18, 19],  # penta15
    }

    def __init__(self, obj):
        self.obj = obj
        self.geom = None
        self.mesh = None
        self.fem_mesh = None
        self.params = meshing.MeshingParameters()
#    if(kwargs.contains("meshsizefilename"))
#      mp.meshsizefilename = py::cast<string>(kwargs.attr("pop")("meshsizefilename"));


    def set_parameters(self):
        if self.obj.Fineness == "UserDefined":
            self.params = meshing.MeshingParameters(
                **self.get_meshing_parameters()
            )
        elif self.obj.Fineness == "VeryCoarse":
            self.params = meshing.meshsize.very_coarse
        elif self.obj.Fineness == "Coarse":
            self.params = meshing.meshsize.coarse
        elif self.obj.Fineness == "Moderate":
            self.params = meshing.meshsize.moderate
        elif self.obj.Fineness == "Fine":
            self.params = meshing.meshsize.fine
        elif self.obj.Fineness == "VeryFine":
            self.params = meshing.meshsize.very_fine

        print(self.params)

    def load_geom(self):
        print("start load geom")
        t0 = time.time()
        tmpdir = tempfile.mkdtemp(prefix="fem_")
        sh = self.obj.Shape.getPropertyOfGeometry()
        brep_file = tmpdir + "/shape.brep"
        sh.exportBrep(brep_file)
        self.geom = occ.OCCGeometry(brep_file)
        os.remove(brep_file)
#        os.remove(tmpdir)
        print("load geom: ", time.time() - t0)

    def compute(self):
        ngcore.SetNumThreads(self.obj.Threads)
        print("start compute")
        t0 = time.time()
        with ngcore.TaskManager():
            if self.obj.HealShape:
                self.geom.Heal()
            self.mesh = self.geom.GenerateMesh(mp=self.params)

        if self.obj.SecondOrder:
            self.mesh.SecondOrder()

        print("compute: ", time.time() - t0)

    def set_fem_mesh(self):
        self.fem_mesh = Fem.FemMesh()

        t0 = time.time()
        for node in self.mesh.Coordinates():
            self.fem_mesh.addNode(*node)
        print("nodos: ", time.time() - t0)

        edges = self.mesh.Elements1D().NumPy()
        faces = self.mesh.Elements2D().NumPy()
        volumes = self.mesh.Elements3D().NumPy()

        nod_edges = edges["nodes"]
        nod_faces = faces["nodes"]
        nod_volumes = volumes["nodes"]

        np_edges = (nod_edges != 0).sum(axis=1).tolist()
        np_faces = faces["np"].tolist()
        np_volumes = volumes["np"].tolist()

        # set smesh node order
        for i in range(faces.size):
            nod_faces[i] = nod_faces[i][self.order_face[np_faces[i]]]

        for i in range(volumes.size):
            nod_volumes[i] = nod_volumes[i][self.order_volume[np_volumes[i]]]

        t0 = time.time()
        flat_edges = nod_edges[nod_edges != 0].tolist()
        self.fem_mesh.addEdgeList(flat_edges, np_edges)
        print("edges: ", time.time() - t0)

        t0 = time.time()
        flat_faces = nod_faces[nod_faces != 0].tolist()
        self.fem_mesh.addFaceList(flat_faces, np_faces)
        print("faces: ", time.time() - t0)

        t0 = time.time()
        flat_volumes = nod_volumes[nod_volumes != 0].tolist()
        self.fem_mesh.addVolumeList(flat_volumes, np_volumes)
        print("volumes: ", time.time() - t0)

        # create groups
        nb_edges = edges.size
        nb_faces = faces.size
        nb_volumes = volumes.size

        idx_edges = edges["index"]
        idx_faces = faces["index"]
        idx_volumes = volumes["index"]

        t0 = time.time()
        for i in np.unique(idx_edges):
            edge_i = (np.nonzero(idx_edges == i)[0] + 1).tolist()
            grp_id = self.fem_mesh.addGroup("Edge" + str(i), "Edge")
            self.fem_mesh.addGroupElements(grp_id, edge_i)
        print("group edges: ", time.time() - t0)

        t0 = time.time()
        for i in np.unique(idx_faces):
            face_i = (np.nonzero(idx_faces == i)[0] + nb_edges + 1).tolist()
            grp_id = self.fem_mesh.addGroup("Face" + str(i), "Face")
            self.fem_mesh.addGroupElements(grp_id, face_i)
        print("group faces: ", time.time() - t0)

        t0 = time.time()
        for i in np.unique(idx_volumes):
            volume_i = (np.nonzero(idx_volumes == i)[0] + nb_edges + nb_faces + 1).tolist()
            grp_id = self.fem_mesh.addGroup("Solid" + str(i), "Volume")
            self.fem_mesh.addGroupElements(grp_id, volume_i)
        print("group volumes: ", time.time() - t0)

        self.obj.FemMesh = self.fem_mesh

        return self.fem_mesh

    def create_mesh(self):
        self.load_geom()
        self.set_parameters()
        self.compute()
        self.set_fem_mesh()

    def get_meshing_parameters(self):
        params = {
            "optimize3d": self.obj.Optimize3d,
            "optimize2d": self.obj.Optimize2d,
            "optsteps3d": self.obj.OptimizationSteps3d,
            "optsteps2d": self.obj.OptimizationSteps2d,
            "opterrpow": self.obj.OptimizationErrorPower,
            "blockfill": self.obj.BlockFill,
            "filldist": self.obj.FillDistance.Value,
            "safety": self.obj.Safety,
            "relinnersafety": self.obj.RelinnerSafety,
            "uselocalh": self.obj.UseLocalH,
            "grading": self.obj.GrowthRate,
   
            "delaunay": self.obj.Delaunay,
            "delaunay2d": self.obj.Delaunay2d,
            "maxh": self.obj.MaxSize.Value,
            "minh": self.obj.MinSize.Value,
            "startinsurface": self.obj.StartInSurface,
            "checkoverlap": self.obj.CheckOverlap,
            "checkoverlappingboundary": self.obj.CheckOverlappingBoundary,
            "checkchartboundary": self.obj.CheckChartBoundary,
            "curvaturesafety": self.obj.CurvatureSafety,
            "segmentsperedge": self.obj.SegmentsPerEdge,
            "elsizeweight": self.obj.ElementSizeWeight,
            "parthread": self.obj.ParallelMeshing,
            "perfstepsstart": self.obj.StartStep,
            "perfstepsend": self.obj.EndStep,
            "giveuptol2d": self.obj.GiveUpTolerance2d,
            "giveuptol": self.obj.GiveUpTolerance,
            "giveuptolopenquads": self.obj.GiveUpToleranceOpenQuads,
#    if(kwargs.contains("maxoutersteps"))
#      mp.maxoutersteps = py::cast<int>(kwargs.attr("pop")("maxoutersteps"));
#    if(kwargs.contains("starshapeclass"))
#      mp.starshapeclass = py::cast<int>(kwargs.attr("pop")("starshapeclass"));
#    if(kwargs.contains("baseelnp"))
#      mp.baseelnp = py::cast<int>(kwargs.attr("pop")("baseelnp"));
#    if(kwargs.contains("sloppy"))
#      mp.sloppy = py::cast<int>(kwargs.attr("pop")("sloppy"));
#    if(kwargs.contains("badellimit"))
#      mp.badellimit = py::cast<double>(kwargs.attr("pop")("badellimit"));
#    if(kwargs.contains("check_impossible"))
#      mp.check_impossible = py::cast<bool>(kwargs.attr("pop")("check_impossible"));
#    if(kwargs.contains("only3D_domain_nr"))
#      mp.only3D_domain_nr = py::cast<int>(kwargs.attr("pop")("only3D_domain_nr"));
            "secondorder": self.obj.SecondOrder,
            "elementorder": self.obj.ElementOrder,
            "quad_dominated": self.obj.QuadDominated,
            "try_hexes": self.obj.TryHexes,

#    if(kwargs.contains("inverttets"))
#      mp.inverttets = py::cast<bool>(kwargs.attr("pop")("inverttets"));
#    if(kwargs.contains("inverttrigs"))
#      mp.inverttrigs = py::cast<bool>(kwargs.attr("pop")("inverttrigs"));
            "autozrefine": self.obj.AutoZRefine,
            "parallel_meshing": self.obj.ParallelMeshing,
            "nthreads": self.obj.Threads,
            "closeedgefac": self.obj.CloseEdgeFactor,
        }

        return params


def netgen_version():
    result = "{:<26}{:>}\n" + "{:<26}{:>}\n" + "{:<26}{:>}\n" + "{:<26}{:>}"
    return result.format(
        "Netgen:",
        ng_config.version,
        "Python:",
        ng_config.PYTHON_VERSION,
        "OCC:",
        occ.occ_version,
        "Use MPI:",
        ng_config.USE_MPI,
    )
