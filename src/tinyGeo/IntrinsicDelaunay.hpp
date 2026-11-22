#ifndef TINYGEO_INTRINSIC_DELAUNAY_HPP
#define TINYGEO_INTRINSIC_DELAUNAY_HPP
#include <tinyGeo/HalfEdge.hpp>
#include <tinyGeo/Mesh.hpp>
#include <tinyGeo/Geometry.hpp>
namespace tinyGeo {
    float cotFromLengths(float opp, float a, float b);
    bool isLocallyDelaunay(HalfEdgeMesh& mesh, int he0);
    float flippedEdgeLength(HalfEdgeMesh& mesh, int halfEdgeToFlip);
    void flipEdgeIntrinsic(HalfEdgeMesh& mesh, int he0);
    void makeIntrinsicDelaunay(HalfEdgeMesh& mesh);
    Mesh intrinsicDelaunay(Mesh input);
}
#endif