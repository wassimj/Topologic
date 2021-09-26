#include <pybind11/pybind11.h>
#include "wrapper_header_collection.hpp"
namespace py = pybind11;

#include "TopologicalQuery.cppwg.hpp"
#include "Topology.cppwg.hpp"
#include "Vertex.cppwg.hpp"
#include "Edge.cppwg.hpp"
#include "Wire.cppwg.hpp"
#include "Face.cppwg.hpp"
#include "Cell.cppwg.hpp"


PYBIND11_MODULE(topologic, m)
{
    register_TopologicalQuery_class(m);
    register_Topology_class(m);
    register_Vertex_class(m);
    register_Edge_class(m);
    register_Wire_class(m);
    register_Face_class(m);
    register_Cell_class(m);
}