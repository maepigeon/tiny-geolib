#include "Geometry.hpp"
namespace tinyGeo {
    // Area of a triangle from its 3 edge lengths Kahan (stable) Heron) https://en.wikipedia.org/wiki/Heron%27s_formula
    float triangleArea(float l0, float l1, float l2) {
        if (l0 < l1) {
            std::swap(l0, l1);
        }
        if (l1 < l2) {
            std::swap(l1, l2);
        }
        if (l0 < l1) {
            std::swap(l0, l1);
        }
        float s = (l0 + (l1 + l2)) * (l2 - (l0 - l1)) * (l2 + (l0 - l1)) * (l0 + (l1 - l2));
        return s > 0.0f ? 0.25f * std::sqrt(s) : 0.0f;   // clamp area to 0 if degenerate
    }
    // Gets the outgoing edges from a vertex
    std::vector<int> outgoingHalfEdges(HalfEdgeMesh& mesh, int halfedgeIndex) {
        std::vector<int> ringAroundTheVertex;
        int cur = halfedgeIndex;
        do {
            ringAroundTheVertex.push_back(cur);
            int twin = mesh.halfEdges[cur].twin;
            if (twin < 0) {
                cur = -1; 
                break; 
            } 
            cur = mesh.halfEdges[twin].next;  // cur.twin().next()
        } while (cur != halfedgeIndex);

        if (cur == halfedgeIndex) {
            return ringAroundTheVertex;
        }
        cur = halfedgeIndex;
        while (true) {
            int prev = mesh.halfEdges[mesh.halfEdges[cur].next].next;  //prev
            int twin = mesh.halfEdges[prev].twin;
            if (twin < 0) {
                break; // other boundary reached
            }
            cur = twin; // twin is outgoing at same vertex
            ringAroundTheVertex.insert(ringAroundTheVertex.begin(), cur); // prepend to keep ring order
        }
        return ringAroundTheVertex;
    }

    // area of the face the halfedge is in
    float faceArea(HalfEdgeMesh& he, int halfedgeIndex) {
        int h0 = halfedgeIndex;
        int h1 = he.halfEdges[h0].next;
        int h2 = he.halfEdges[h1].next;
        return triangleArea(he.halfEdges[h0].length, he.halfEdges[h1].length, he.halfEdges[h2].length);
    }
    
    //Computes the barycentric dual area of a vertex.
    float barycentricDualArea(HalfEdgeMesh mesh, int vertexHalfEdgeIndex) {
        buildVertexToHEvector(mesh);
        std::vector<int> outgoing = outgoingHalfEdges(mesh, vertexHalfEdgeIndex);
        // One third of the summed area of triangles touching he's start-vertex
        double sumOfAreas = 0;
        for (int he : outgoing) {
            sumOfAreas += faceArea(mesh, he);
        }
        return sumOfAreas / 3.;
    }
}