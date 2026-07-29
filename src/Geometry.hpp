#ifndef TINYGEO_GEOMETRY
#define TINYGEO_GEOMETRY
#include "Mesh.hpp"
#include "HalfEdge.hpp"
#include <vector>
namespace tinyGeo {
    float triangleArea(float l0, float l1, float l2);
    std::vector<int> outgoingHalfEdges(HalfEdgeMesh& mesh, int halfedgeIndex);
    float faceArea(HalfEdgeMesh& he, int halfedgeIndex);
    float barycentricDualArea(HalfEdgeMesh mesh, int vertexHalfEdgeIndex);
}
#endif