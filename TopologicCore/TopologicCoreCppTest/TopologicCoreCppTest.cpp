// TopologicCoreCppTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <Vertex.h>
#include <Wire.h>
#include <Face.h>
#include <Cell.h>
#include <CellComplex.h>

using namespace TopologicCore;
Topology::Ptr CellComplexByCells(std::list<Cell::Ptr>&& cells);


int main()
{
    std::cout << "Runtime test of Topologic Core library\n";

    // Creating vertices
    auto v1 = Vertex::ByCoordinates(0, 0, 0);
    auto v2 = Vertex::ByCoordinates(10, 0, 0);
    auto v3 = Vertex::ByCoordinates(10, 10, 0);
    auto v4 = Vertex::ByCoordinates(0, 10, 0);
    auto v5 = Vertex::ByCoordinates(0, 0, 10);
    auto v6 = Vertex::ByCoordinates(10, 0, 10);
    auto v7 = Vertex::ByCoordinates(10, 10, 10);
    auto v8 = Vertex::ByCoordinates(0, 10, 10);
    auto v9 = Vertex::ByCoordinates(0, 0, 20);
    auto v10 = Vertex::ByCoordinates(10, 0, 20);
    auto v11 = Vertex::ByCoordinates(10, 10, 20);
    auto v12 = Vertex::ByCoordinates(0, 10, 20);

    // Creating edges
    auto e1 = Edge::ByStartVertexEndVertex(v1, v2);
    auto e2 = Edge::ByStartVertexEndVertex(v2, v3);
    auto e3 = Edge::ByStartVertexEndVertex(v3, v4);
    auto e4 = Edge::ByStartVertexEndVertex(v4, v1);
    auto e5 = Edge::ByStartVertexEndVertex(v5, v6);
    auto e6 = Edge::ByStartVertexEndVertex(v6, v7);
    auto e7 = Edge::ByStartVertexEndVertex(v7, v8);
    auto e8 = Edge::ByStartVertexEndVertex(v8, v5);
    auto e9 = Edge::ByStartVertexEndVertex(v1, v5);
    auto e10 = Edge::ByStartVertexEndVertex(v2, v6);
    auto e11 = Edge::ByStartVertexEndVertex(v3, v7);
    auto e12 = Edge::ByStartVertexEndVertex(v4, v8);
    auto e13 = Edge::ByStartVertexEndVertex(v9, v10);
    auto e14 = Edge::ByStartVertexEndVertex(v10, v11);
    auto e15 = Edge::ByStartVertexEndVertex(v11, v12);
    auto e16 = Edge::ByStartVertexEndVertex(v12, v9);
    auto e17 = Edge::ByStartVertexEndVertex(v5, v9);
    auto e18 = Edge::ByStartVertexEndVertex(v6, v10);
    auto e19 = Edge::ByStartVertexEndVertex(v7, v11);
    auto e20 = Edge::ByStartVertexEndVertex(v8, v12);

    std::vector<Edge::Ptr> edges = { e1, e2, e3, e4, e5 };
    for (auto anEdge : edges) {
        std::cout << anEdge << "\n";
        std::list<Vertex::Ptr> vertices;
        anEdge->Vertices(anEdge, vertices);
        for (auto aVertex : vertices) {
            std::cout << aVertex->X() << " " << aVertex->Y() << " " << aVertex->Z() << "\n";
        }
    }
    std::cout << "Done\n";

    // create wires
    std::cout << "Creating Wires...\n";
    auto w1 = Wire::ByEdges({ e1,e4,e3,e2 });
    auto w2 = Wire::ByEdges({ e5,e6,e7,e8 });
    auto w3 = Wire::ByEdges({ e1,e10,e5,e9 });
    auto w4 = Wire::ByEdges({ e2,e11,e6,e10 });
    auto w5 = Wire::ByEdges({ e3,e12,e7,e11 });
    auto w6 = Wire::ByEdges({ e4,e9,e8,e12 });
    auto w7 = Wire::ByEdges({ e13,e14,e15,e16 });
    auto w8 = Wire::ByEdges({ e5,e18,e13,e17 });
    auto w9 = Wire::ByEdges({ e6,e19,e14,e18 });
    auto w10 = Wire::ByEdges({ e7,e20,e15,e19 });
    auto w11 = Wire::ByEdges({ e8,e17,e16,e20 });
    std::cout << "Done\n";

    // Test getting vertices from wires
    std::cout << "Testing getting Vertices from Wires...\n";
    std::vector<Wire::Ptr> wires = { w1, w2, w3, w4, w5 };
    for (auto aWire : wires) {
        std::cout << aWire << "\n";
        std::list<Vertex::Ptr> vertices;
        aWire->Vertices(aWire, vertices);
        for (auto aVertex : vertices) {
            std::cout << aVertex->X() << " " << aVertex->Y() << " " << aVertex->Z() << "\n";
            std::list<Edge::Ptr> pEdges;
            aVertex->Edges(aWire, pEdges);
            for (auto aPEdge : pEdges) {
                std::cout << "   " << aPEdge << "\n";
            }
        }
    }
    std::cout << "Done\n";

    // Test getting Edges from Wires
    std::cout << "Testing getting Edges from Wires...\n";
    wires = { w1, w2, w3, w4, w5 };
    for (auto aWire : wires) {
        std::cout << aWire << "\n";
        std::list<Edge::Ptr> edges;
        aWire->Edges(aWire, edges);
        for (auto anEdge : edges) {
            std::cout << "   " << anEdge << "\n";
            std::list<Edge::Ptr> adjEdges;
            anEdge->AdjacentEdges(aWire, adjEdges);
            std::list<Vertex::Ptr> sharedVertices;
            std::cout << "--\n";
            // std::cout << adjEdges << "\n"; // Missing << override
            anEdge->SharedVertices(adjEdges.front(), sharedVertices);
            std::cout << sharedVertices.size() << "\n";
            for (auto sharedVertex : sharedVertices) {
                std::cout << "      Shared Vertices: " << sharedVertex << "\n";
            }
            for (auto adjEdge : adjEdges) {
                std::cout << "      Adjacent Edges: " << adjEdge << "\n";
            }
        }
    }
    std::cout << "Done\n";

    // Create faces
    std::cout << "Creating Faces...\n";
    auto f1 = Face::ByExternalBoundary(w1);
    auto f2 = Face::ByExternalInternalBoundaries(w2, {});
    auto f3 = Face::ByExternalInternalBoundaries(w3, {});
    auto f4 = Face::ByExternalInternalBoundaries(w4, {});
    auto f5 = Face::ByExternalInternalBoundaries(w5, {});
    auto f6 = Face::ByExternalInternalBoundaries(w6, {});
    auto f7 = Face::ByExternalBoundary(w7);
    auto f8 = Face::ByExternalInternalBoundaries(w8, {});
    auto f9 = Face::ByExternalInternalBoundaries(w9, {});
    auto f10 = Face::ByExternalInternalBoundaries(w10, {});
    auto f11 = Face::ByExternalInternalBoundaries(w11, {});
    std::cout << "Done\n";

    // Create cells
    std::cout << "Creating Cells...\n";
    auto c1 = Cell::ByFaces({ f1,f2,f3,f4,f5,f6 }, 0.001);
    auto c2 = Cell::ByFaces({ f2,f7,f8,f9,f10,f11 }, 0.001);
    std::cout << "Done\n";

    // Create cellComplex
    std::cout << "Creating CellComplex...\n";
    Topology::Ptr cc1 = CellComplexByCells({ c1, c2 });
    const std::string path = "C:\\Users\\info\\Downloads\\CellComplex.brep";
    cc1->ExportToBRep(path, 3);
    std::cout << "BREP file exported to: " << path << "\n";
    std::cout << "Done\n";

    std::cout << "Congratulations! Test is successful.\n";

    return 0;
}

Topology::Ptr CellComplexByCells(std::list<Cell::Ptr>&& cells)
{
    std::vector<Cell::Ptr> cellsVector = std::vector<Cell::Ptr>(cells.begin(), cells.end());
    Topology::Ptr cellComplex = cellsVector[0];

    const int cellComplexSize = cellsVector.size();
    for (int i = 1; i < cellComplexSize; ++i)
    {
        try
        {
            cellComplex = cellComplex->Merge(cellsVector[i], false);
        }
        catch (std::exception& ex)
        {
            std::cout << "Exception: " << ex.what() << std::endl;
            throw std::exception("Known Error: CellComplex.ByCells operation failed during processing the input Cells");
        }
        catch (...)
        {
            std::cout << "Unknown exception occured!" << std::endl;
            throw std::exception("Unknown Error: CellComplex.ByCells operation failed during processing the input Cells");
        }

        auto topologyType = cellComplex->GetType();
        if (topologyType != 64)
        {
            std::cout << "Warning: Input cells do not form a cell complex" << std::endl;
        }
        return cellComplex;
    }
}