#include <pybind11/pybind11.h>
#include "wrapper_header_collection.hpp"
namespace py = pybind11;

#include "TopologicalQuery.cppwg.hpp"
#include "Topology.cppwg.hpp"
#include "Vertex.cppwg.hpp"
#include "Edge.cppwg.hpp"


PYBIND11_MODULE(topologic, m)
{
    register_TopologicalQuery_class(m);
    register_Topology_class(m);
    register_Vertex_class(m);
    register_Edge_class(m);
}