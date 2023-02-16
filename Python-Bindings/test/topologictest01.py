import sys
import os
from os.path import expanduser
home = expanduser("~")
release = os.path.join(home, "topologicbim", "Topologic", "output", "x64", "Release")
print(release)
sys.path.append(release)

from topologic import Vertex, Edge, Wire, Face, Shell, Cell, CellComplex, Cluster, Aperture, Dictionary, Topology

def classByType(argument):
	switcher = {
		1: Vertex,
		2: Edge,
		4: Wire,
		8: Face,
		16: Shell,
		32: Cell,
		64: CellComplex,
		128: Cluster }
	return switcher.get(argument, Topology)

def fixTopologyClass(topology):
  topology.__class__ = classByType(topology.Type())
  return topology

def cellComplexByCells(cells):
	cellComplex = cells[0]
	for i in range(1,len(cells)):
		try:
			cellComplex = cellComplex.Merge(cells[i], False)
		except:
			raise Exception("Error: CellComplex.ByCells operation failed during processing the input Cells")
	if cellComplex.Type() != 64: #64 is the type of a CellComplex
		warnings.warn("Warning: Input Cells do not form a CellComplex", UserWarning)
	return fixTopologyClass(cellComplex)

print("Starting Test...")
# create vertices
print("Creating Vertices...")
v1 = Vertex.ByCoordinates(0,0,0)
v2 = Vertex.ByCoordinates(10,0,0)
v3 = Vertex.ByCoordinates(10,10,0)
v4 = Vertex.ByCoordinates(0,10,0)
v5 = Vertex.ByCoordinates(0,0,10) 
v6 = Vertex.ByCoordinates(10,0,10)
v7 = Vertex.ByCoordinates(10,10,10)
v8 = Vertex.ByCoordinates(0,10,10)
v9 = Vertex.ByCoordinates(0,0,20) 
v10 = Vertex.ByCoordinates(10,0,20)
v11 = Vertex.ByCoordinates(10,10,20)
v12 = Vertex.ByCoordinates(0,10,20)
print("Done")

# create edges
print("Creating Edges...")
e1 = Edge.ByStartVertexEndVertex(v1, v2)
e2 = Edge.ByStartVertexEndVertex(v2, v3)
e3 = Edge.ByStartVertexEndVertex(v3, v4)
e4 = Edge.ByStartVertexEndVertex(v4, v1)
e5 = Edge.ByStartVertexEndVertex(v5, v6)
e6 = Edge.ByStartVertexEndVertex(v6, v7)
e7 = Edge.ByStartVertexEndVertex(v7, v8)
e8 = Edge.ByStartVertexEndVertex(v8, v5)
e9 = Edge.ByStartVertexEndVertex(v1, v5)
e10 = Edge.ByStartVertexEndVertex(v2, v6)
e11 = Edge.ByStartVertexEndVertex(v3, v7)
e12 = Edge.ByStartVertexEndVertex(v4, v8)
e13 = Edge.ByStartVertexEndVertex(v9, v10)
e14 = Edge.ByStartVertexEndVertex(v10, v11)
e15 = Edge.ByStartVertexEndVertex(v11, v12)
e16 = Edge.ByStartVertexEndVertex(v12, v9)
e17 = Edge.ByStartVertexEndVertex(v5, v9)
e18 = Edge.ByStartVertexEndVertex(v6, v10)
e19 = Edge.ByStartVertexEndVertex(v7, v11)
e20 = Edge.ByStartVertexEndVertex(v8, v12)
print("Done")

# Test getting vertices from edges
print("Testing getting Vertices from Edges...")
edges = [e1, e2, e3, e4, e5]
for anEdge in edges:
    print(anEdge)
    vertices = []
    anEdge.Vertices(vertices)
    for aVertex in vertices:
        print(aVertex.X(), aVertex.Y(), aVertex.Z())
print("Done")
# create wires
print("Creating Wires...")
w1 = Wire.ByEdges([e1,e4,e3,e2])
w2 = Wire.ByEdges([e5,e6,e7,e8])
w3 = Wire.ByEdges([e1,e10,e5,e9])
w4 = Wire.ByEdges([e2,e11,e6,e10])
w5 = Wire.ByEdges([e3,e12,e7,e11])
w6 = Wire.ByEdges([e4,e9,e8,e12])
w7 = Wire.ByEdges([e13,e14,e15,e16])
w8 = Wire.ByEdges([e5,e18,e13,e17])
w9 = Wire.ByEdges([e6,e19,e14,e18])
w10 = Wire.ByEdges([e7,e20,e15,e19])
w11 = Wire.ByEdges([e8,e17,e16,e20])
print("Done")

# Test getting vertices from wires
print("Testing getting Vertices from Wires...")
wires = [w1, w2, w3, w4, w5]
for aWire in wires:
    print(aWire)
    vertices = []
    aWire.Vertices(vertices)
    for aVertex in vertices:
        print(aVertex.X(), aVertex.Y(), aVertex.Z())
        pEdges = []
        aVertex.Edges(pEdges)
        for aPEdge in pEdges:
            print("   "+str(aPEdge))
print("Done")
# Test getting Edges from Wires
print("Testing getting Edges from Wires...")
wires = [w1, w2, w3, w4, w5]
for aWire in wires:
    print(aWire)
    edges = []
    aWire.Edges(edges)
    for anEdge in edges:
        print("   "+str(anEdge))
        adjEdges = []
        anEdge.AdjacentEdges(adjEdges)
        sharedVertices = []
        anEdge.SharedVertices(adjEdges[0], sharedVertices)
        print(len(sharedVertices))
        for sharedVertex in sharedVertices:
            print("      Shared Vertices: "+str(sharedVertex))
        for adjEdge in adjEdges:
            print("      Adjacent Edges: "+str(adjEdge))
print("Done")
# create faces
print("Creating Faces...")
f1 = Face.ByExternalBoundary(w1)
f2 = Face.ByExternalInternalBoundaries(w2, [])
f3 = Face.ByExternalInternalBoundaries(w3, [])
f4 = Face.ByExternalInternalBoundaries(w4, [])
f5 = Face.ByExternalInternalBoundaries(w5, [])
f6 = Face.ByExternalInternalBoundaries(w6, [])
f7 = Face.ByExternalBoundary(w7)
f8 = Face.ByExternalInternalBoundaries(w8, [])
f9 = Face.ByExternalInternalBoundaries(w9, [])
f10 = Face.ByExternalInternalBoundaries(w10, [])
f11 = Face.ByExternalInternalBoundaries(w11, [])
print("Done")

# create cells
print("Creating Cells...")
c1 = Cell.ByFaces([f1,f2,f3,f4,f5,f6],0.001)
c2 = Cell.ByFaces([f2,f7,f8,f9,f10,f11],0.001)
print("Done")

# Creat cellComplex
print("Creating CellComplex...")
cc1 = cellComplexByCells([c1, c2])
path = os.path.join(home, "CellComplex.brep")
cc1.ExportToBRep(path)
print("BREP file exported to: "+path)
print("Done")
print("Congratulations! Test is successful.")
