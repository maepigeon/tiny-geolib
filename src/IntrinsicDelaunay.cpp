#include <cmath>
#include <algorithm>
#include <vector>
#include <cstdio>

#include "IntrinsicDelaunay.hpp"
#include "HalfEdge.hpp"
#include "Mesh.hpp"
#include "HalfEdge.hpp"

namespace tinyGeo {
    // is edge he0 locally Delaunay? if false: flip it
    bool isLocallyDelaunay(HalfEdgeMesh& mesh, int he0) {
        int he3 = mesh.halfEdges[he0].twin;
        int he1 = mesh.halfEdges[he0].next;
        int he2 = mesh.halfEdges[he1].next;
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
        float cot2 = cotan(mesh, he2);  // cot of angle at v2
        float cot5 = cotan(mesh, he5);  // cot of angle at v5
        return (cot2 + cot5) >= 0.0f;
    }
    // length of the flipped edge (v2->v3), by unfolding both faces at v0
    float flipLength(HalfEdgeMesh& mesh, int he0) {
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
        float cos0 = (e0 * e0 + e2 * e2 - e1 * e1) / (2.0f * e0 * e2);  // cos angle at v0, face 0
        float cos1 = (e0 * e0 + e4 * e4 - e5 * e5) / (2.0f * e0 * e4);  // cos angle at v0, face 1
        cos0 = cos0 > -1.0f ? (cos0 < 1.0f ? cos0 : 1.0f) : -1.0f;
        cos1 = cos1 > -1.0f ? (cos1 < 1.0f ? cos1 : 1.0f) : -1.0f;
        float sq0 = std::sqrt(1.0f - cos0 * cos0);
        float sq1 = std::sqrt(1.0f - cos1 * cos1);
        float cosSum = cos0 * cos1 - sq0 * sq1; // cos(angle v2-v0-v3)
        float d2 = e2 * e2 + e4 * e4 - 2.0f * e2 * e4 * cosSum; // law of cosines for |v2 v3|
        return d2 > 0.0f ? std::sqrt(d2) : 0.0f;
    }

    void flipEdgeIntrinsic(HalfEdgeMesh& mesh, int he0) {
        int he3 = mesh.halfEdges[he0].twin;
        int he1 = mesh.halfEdges[he0].next;
        int he2 = mesh.halfEdges[he1].next;
        int he4 = mesh.halfEdges[he3].next;
        int he5 = mesh.halfEdges[he4].next;
        int f0 = mesh.halfEdges[he0].face, f1 = mesh.halfEdges[he3].face;
        int v2 = mesh.halfEdges[he2].origin;
        int v3 = mesh.halfEdges[he5].origin;
        float newLen = flipLength(mesh, he0); 
        mesh.halfEdges[he0].origin = v3;
        mesh.halfEdges[he3].origin = v2;
        mesh.halfEdges[he0].length = newLen;
        mesh.halfEdges[he3].length = newLen;
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
        mesh.faceToHE[f0] = he0;
        mesh.faceToHE[f1] = he3;
    }

    void makeIntrinsicDelaunay(HalfEdgeMesh& mesh) {
        int n = mesh.halfEdges.size();
        std::vector<int>  stack;
        std::vector<char> inStack(n, 0);
        for (int h = 0; h < n; h++) {
            int t = mesh.halfEdges[h].twin;
            if (t < 0) {
                continue; // boundary edge
            }
            int c = h < t ? h : t;
            if (!inStack[c]) {
                stack.push_back(c);
                 inStack[c] = 1; 
            }
        }
        while (!stack.empty()) {
            int he0 = stack.back(); stack.pop_back(); inStack[he0] = 0;
            if (mesh.halfEdges[he0].twin < 0) {
                continue;
            }
            if (!isLocallyDelaunay(mesh, he0)) {
                int he3 = mesh.halfEdges[he0].twin;
                flipEdgeIntrinsic(mesh, he0);
                int quadedge0 = mesh.halfEdges[he0].next; 
                int quadedge1 = mesh.halfEdges[quadedge0].next;
                int quadedge2 = mesh.halfEdges[he3].next;
                int quadedge3 = mesh.halfEdges[quadedge2].next;
                int quad[4] = { quadedge0, quadedge1, quadedge2, quadedge3 };
                for (int s = 0; s < 4; s++) {
                    int t = mesh.halfEdges[quad[s]].twin;
                    if (t < 0) {
                        continue;
                    }
                    int c = quad[s] < t ? quad[s] : t;
                    if (!inStack[c]) {
                        stack.push_back(c);
                        inStack[c] = 1; 
                    }
                }
            }
        }
    }
    Mesh intrinsicDelaunay(Mesh input) {
        HalfEdgeMesh mesh = buildHalfEdgeMesh(input);
        if (mesh.isNonManifold) {
            std::printf("[warn] non-manifold input; iDT assumes manifold\n");
        }
        initializeIntrinsicLengths(mesh);
        makeIntrinsicDelaunay(mesh);
        buildVertexToHE(mesh);
        return toMesh(mesh);
    }
}