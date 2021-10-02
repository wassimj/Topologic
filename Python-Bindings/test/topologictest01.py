import sys
sys.path.append(r"C:\Users\wassimj\topologicbim\Topologic\output\x64\Release")

from topologic import Vertex, Edge, Wire, Face, Cell, CellComplex, Cluster, Aperture, Dictionary, Topology
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
e8 = Edge.ByStartVertexEndVertex(v8, v1)
e9 = Edge.ByStartVertexEndVertex(v1, v5)
e10 = Edge.ByStartVertexEndVertex(v2, v6)
e11 = Edge.ByStartVertexEndVertex(v3, v7)
e12 = Edge.ByStartVertexEndVertex(v4, v8)
print(e12)
print("Done")

# create wires
print("Creating Wires...")
w1 = Wire.ByEdges([e1,e2,e3,e4])
print("Congratulations! Test is succesful.")
