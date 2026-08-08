#ifndef INTRINSIC_DELAUNAY
#define INTRINSIC_DELAUNAY
#include "HalfEdge.hpp"
#include "Mesh.hpp"
#include "Geometry.hpp"
namespace tinyGeo {
    bool isLocallyDelaunay(HalfEdgeMesh& mesh, int he0);
    float flippedEdgeLength(HalfEdgeMesh& mesh, int halfEdgeToFlip);
    void flipEdgeIntrinsic(HalfEdgeMesh& mesh, int he0);
    void makeIntrinsicDelaunay(HalfEdgeMesh& mesh);
    Mesh intrinsicDelaunay(Mesh input);
}
#endif