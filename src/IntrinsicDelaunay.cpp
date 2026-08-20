#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>
#include "IntrinsicDelaunay.hpp"
#include "HalfEdge.hpp"
#include "Mesh.hpp"
#include "HalfEdge.hpp"

namespace tinyGeo {
    float cotFromLengths(float opp, float a, float b) {
        float area = triangleArea(opp, a, b);
        if (area <= 0.0f) {
            return 0.0f;
        }
        // cos = (a^2 + b^2 - opp^2)/(2ab)
        // sin = 2*area/(ab)
        // cot = cos/sin
        return (a * a + b * b - opp * opp) / (4.0f * area);
    }

    // is edge he0 locally Delaunay? if false: flip it
    bool isEdgeDelaunay(HalfEdgeMesh& mesh, int he0) {
        int he1 = mesh.halfEdges[he0].next;
        int he2 = mesh.halfEdges[he1].next;

        int he3 = mesh.halfEdges[he0].twin;
        int he4 = mesh.halfEdges[he3].next;
        int he5 = mesh.halfEdges[he4].next;

        float e0 = mesh.halfEdges[he0].length;
        float e1 = mesh.halfEdges[he1].length;
        float e2 = mesh.halfEdges[he2].length;
        float e4 = mesh.halfEdges[he4].length;
        float e5 = mesh.halfEdges[he5].length;
        float area0 = triangleArea(e0, e1, e2);   // face 0 (v0,v1,v2) 
        float area1 = triangleArea(e0, e4, e5);   // face 1 (v1,v0,v3)
        if (area0 <= 0.0f || area1 <= 0.0f) {
            return true; // degenerate: don't flip
        }
        float cot2 = cotFromLengths(e0, e1, e2);  // cot of angle at v2
        float cot5 = cotFromLengths(e0, e4, e5);  // cot of angle at v5
        return (cot2 + cot5) >= 0.0f;
    }

    float flippedEdgeLength(HalfEdgeMesh& mesh, int halfEdgeToFlip) {
        int he0 = halfEdgeToFlip;
        int he1 = mesh.halfEdges[he0].next;
        int he2 = mesh.halfEdges[he1].next;

        int he3 = mesh.halfEdges[he0].twin;
        int he4 = mesh.halfEdges[he3].next;
        int he5 = mesh.halfEdges[he4].next;

        float e0_and_e3 = mesh.halfEdges[he0].length; // v0-v1
        float e1 = mesh.halfEdges[he1].length; // v1-v2
        float e2 = mesh.halfEdges[he2].length; // v2-v0
        float e4 = mesh.halfEdges[he4].length; // v0-v3
        float e5 = mesh.halfEdges[he5].length; // v3-v1
        // e1,e2,e4,e5 are the outer quad, e0_and_e3 should be flipped
        // For intrinsic delaunay edge flip, the new edge follows the intrinsic surface
        // Thus we consider the edge flip length to be the length of the quad unfolded and mapped onto a 2D plane,
        // And then we take the distance between them
        float L = e0_and_e3;

        glm::vec2 v0(0.0f, 0.0f);
        glm::vec2 v1(L, 0.0f);

        // v2
        // x = (L^2 + e2^2 - e1^2) / (2L) and y = sqrt(e2^2 - x^2)
        float x2 = (L * L + e2 * e2 - e1 * e1) / (2.0f * L);
        float y2 = std::sqrt(std::max(0.0f, e2 * e2 - x2 * x2));
        glm::vec2 v2(x2, y2);
        //v3
        // x = (L^2 + e4^2 - e5^2) / (2L) and y = -sqrt(e4^2 - x^2)
        float x3 = (L * L + e4 * e4 - e5 * e5) / (2.0f * L);
        float y3 = -std::sqrt(std::max(0.0f, e4 * e4 - x3 * x3));
        glm::vec2 v3(x3, y3);

        // The flipped edge is the other diagonal of the quad: v2-v3.
        return glm::length(v2 - v3);
    }

    void flipEdgeIntrinsic(HalfEdgeMesh& mesh, int he0) {
        int he1 = mesh.halfEdges[he0].next;
        int he2 = mesh.halfEdges[he1].next;
        int he3 = mesh.halfEdges[he0].twin;
        int he4 = mesh.halfEdges[he3].next;
        int he5 = mesh.halfEdges[he4].next;

        int f0 = mesh.halfEdges[he0].face;
        int f1 = mesh.halfEdges[he3].face;

        int v2 = mesh.halfEdges[he2].origin;
        int v3 = mesh.halfEdges[he5].origin;

        float newLength = flippedEdgeLength(mesh, he0);

        mesh.halfEdges[he0].origin = v3;
        mesh.halfEdges[he3].origin = v2;

        mesh.halfEdges[he0].length = newLength;
        mesh.halfEdges[he3].length = newLength;

        // re-order the new half-edge faces
        mesh.halfEdges[he2].next = he4;
        mesh.halfEdges[he4].next = he0;
        mesh.halfEdges[he0].next = he2;
        mesh.halfEdges[he2].face = f0;
        mesh.halfEdges[he4].face = f0;
        mesh.halfEdges[he0].face = f0;

        mesh.halfEdges[he3].next = he5;
        mesh.halfEdges[he5].next = he1;
        mesh.halfEdges[he1].next = he3;
        mesh.halfEdges[he3].face = f1;
        mesh.halfEdges[he5].face = f1;
        mesh.halfEdges[he1].face = f1;
    }

    void makeIntrinsicDelaunay(HalfEdgeMesh& mesh) {
        int halfEdgeCount = mesh.halfEdges.size();
        std::vector<int> stack;
        std::vector<bool> markedEdges(halfEdgeCount, false);
        for (int he = 0; he < halfEdgeCount; he++) {
            int twin = mesh.halfEdges[he].twin;
            if (twin < 0) {
                continue; // boundary edge
            }
            if (he < twin) {
                stack.push_back(he);
                markedEdges[he] = false;
            }
        }
        while (!stack.empty()) {
            int e_ij = stack.back();
            markedEdges[e_ij] = false;  
            stack.pop_back();
            if (mesh.halfEdges[e_ij].twin < 0) {
                continue;
            }
            if (!isEdgeDelaunay(mesh, e_ij)) {
                int he3 = mesh.halfEdges[e_ij].twin;
                flipEdgeIntrinsic(mesh, e_ij);
                int quadedge0 = mesh.halfEdges[e_ij].next; 
                int quadedge1 = mesh.halfEdges[quadedge0].next;
                int quadedge2 = mesh.halfEdges[he3].next;
                int quadedge3 = mesh.halfEdges[quadedge2].next;
                int quad[4] = { quadedge0, quadedge1, quadedge2, quadedge3 };
                for (int i = 0; i < 4; i++) {
                    int twin = mesh.halfEdges[quad[i]].twin;
                    if (twin < 0) {
                        continue;
                    }
                    int theEdge = quad[i] < twin ? quad[i] : twin;
                    if (!markedEdges[theEdge]) {
                        stack.push_back(theEdge);
                        markedEdges[theEdge] = true; 
                    }
                }
            }
        }
    }
    Mesh intrinsicDelaunay(Mesh input) {
        HalfEdgeMesh mesh = buildHalfEdgeMesh(input);
        if (mesh.isNonManifold) {
            std::cout << "Issue: non-manifold input" << std::endl;
        }
        calculateEdgeLengths(mesh);
        makeIntrinsicDelaunay(mesh);
        buildVertexToHEvector(mesh);
        return toMesh(mesh);
    }
}