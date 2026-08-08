#include "HalfEdge.hpp"
#include <map>
#include <utility>
#include <cstdio>
#include "glm/glm.hpp"
#include <iostream>

namespace tinyGeo {
    // Return false if the HalfEdge is on the boundary
    bool isInterior(HalfEdgeMesh& mesh, int he) {
        return mesh.halfEdges[he].twin >= 0;
    }
    // Computes the cotangent of the angle opposite to a halfedge
    float cotan(HalfEdgeMesh& mesh, int he) {
        if (!isInterior(mesh, he)) {
            return 0.;
        }
        // cos = (a dot b) / (norm a * norm b);
        // sin = (a cross b) / (norm a * norm b);
        // cotan = cos / sin = (a dot b) / (a cross b)
        int v0i = he;
        int v1i = mesh.halfEdges[v0i].next;
        int v2i = mesh.halfEdges[v1i].next;
        glm::vec3 v0 = mesh.positions[mesh.halfEdges[v0i].origin];
        glm::vec3 v1 = mesh.positions[mesh.halfEdges[v1i].origin];
        glm::vec3 v2 = mesh.positions[mesh.halfEdges[v2i].origin];
        glm::vec3 v2v0 = v0 - v2;
        glm::vec3 v2v1 = v1 - v2;
        return glm::dot(v2v0, v2v1) / glm::length(glm::cross(v2v0, v2v1));
    }

    void calculateEdgeLengths(HalfEdgeMesh& he) {
        for (int i = 0; i < he.halfEdges.size(); i++) {
            int origin = he.halfEdges[i].origin;
            int endPoint = he.halfEdges[he.halfEdges[i].next].origin;
            he.halfEdges[i].length = glm::length(he.positions[endPoint] - he.positions[origin]);
        }
    }
    HalfEdgeMesh buildHalfEdgeMesh(Mesh mesh) {
        HalfEdgeMesh he;
        he.positions.reserve(mesh.vertices.size());
        he.vertexToHE.assign(mesh.vertices.size(), -1);
        for (int i = 0; i < mesh.vertices.size(); i++) {
            he.positions.push_back(mesh.vertices[i].position);
        }
        int triCount = mesh.indices.size() / 3;
        he.halfEdges.reserve(triCount * 3);
        he.faceToHE.reserve(triCount);

        // create 3 half-edges per triangle
        std::map<std::pair<int,int>, int> edgeMap;
        for (int f = 0; f < triCount; f++) {
            int v0 = mesh.indices[f * 3 + 0];
            int v1 = mesh.indices[f * 3 + 1];
            int v2 = mesh.indices[f * 3 + 2];
            int base = he.halfEdges.size();
            HalfEdge h0 = { v0, -1, base + 1, f }; // origin, twin, next, face
            HalfEdge h1 = { v1, -1, base + 2, f };
            HalfEdge h2 = { v2, -1, base + 0, f };
            he.halfEdges.push_back(h0);
            he.halfEdges.push_back(h1);
            he.halfEdges.push_back(h2);
            he.faceToHE.push_back(base);
            if (he.vertexToHE[v0] < 0) {
                he.vertexToHE[v0] = base;
            }
            if (he.vertexToHE[v1] < 0) {
                he.vertexToHE[v1] = base + 1;
            }
            if (he.vertexToHE[v2] < 0) {
                he.vertexToHE[v2] = base + 2;
            }
            int startPoints[3] = { v0, v1, v2 };
            int endPoints[3] = { v1, v2, v0 };
            for (int v = 0; v < 3; v++) {
                std::pair<int,int> newEdge = { startPoints[v], endPoints[v] };
                if (edgeMap.count(newEdge) > 0) {
                    he.isNonManifold = true;
                    std::cout << "Duplicate directed edge: " << startPoints[v] << ", " << endPoints[v] << std::endl;
                }
                edgeMap[newEdge] = base + v;
        }}
        // pair twins by looking up the reverse edge
        for (int i = 0; i < he.halfEdges.size(); i++) { 
            int origin = he.halfEdges[i].origin;
            int endPoint = he.halfEdges[he.halfEdges[i].next].origin; 
            std::pair<int,int> reverse = { endPoint, origin };
            if (edgeMap.count(reverse)) {
                he.halfEdges[i].twin = edgeMap[reverse];
            }
        }
        return he;
    }

    void buildVertexToHEvector(HalfEdgeMesh& mesh) {
        mesh.vertexToHE.assign(mesh.positions.size(), -1);
        for (int i = 0; i < mesh.halfEdges.size(); i++) {
            int v = mesh.halfEdges[i].origin;
            if (mesh.vertexToHE[v] < 0) {
                mesh.vertexToHE[v] = i;
            }
        }
    }

    Mesh toMesh(HalfEdgeMesh he) {
        Mesh out;
        out.vertices.reserve(he.positions.size());
        for (int i = 0; i < he.positions.size(); i++) {
            out.vertices.push_back({ he.positions[i] });
        }
        out.indices.reserve(he.faceToHE.size() * 3);
        for (int f = 0; f < he.faceToHE.size(); f++) {
            int h = he.faceToHE[f];
            out.indices.push_back(he.halfEdges[h].origin);
            out.indices.push_back(he.halfEdges[he.halfEdges[h].next].origin);
            out.indices.push_back(he.halfEdges[he.halfEdges[he.halfEdges[h].next].next].origin);
        }
        out.computeNormals();
        return out;
    }
}