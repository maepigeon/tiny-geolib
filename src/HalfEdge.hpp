#ifndef TINYGEO_HALFEDGE
#define TINYGEO_HALFEDGE
#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

namespace tinyGeo {
    struct HalfEdge {
        int origin; // vertex that this half-edge starts at
        int twin; // the opposite direction half-edge
        int next;
        int face; // triangle that this half-edge is in
        double length;
    };
    struct HalfEdgeMesh {
        std::vector<glm::vec3> positions;
        std::vector<HalfEdge> halfEdges;
        std::vector<int> faceToHE;
        std::vector<int> vertexToHE;
        bool isNonManifold = false;
        int heOfFace(int f) { return faceToHE[f]; } // the three half-edges of a triangle, in order
    };
    void calculateEdgeLengths(HalfEdgeMesh& he);
    HalfEdgeMesh buildHalfEdgeMesh(Mesh mesh);
    void buildVertexToHEvector(HalfEdgeMesh& mesh);
    Mesh toMesh(HalfEdgeMesh he);
    float cotan(HalfEdgeMesh& mesh, int he);
    bool isInterior(HalfEdgeMesh& mesh, int he);
} 
#endif