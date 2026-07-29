#include "DiffGeo.hpp"
#include <vector>
#include <utility>
#include "Geometry.hpp"
#include "HalfEdge.hpp"


namespace tinyGeo {
    // Builds a positive-definite laplacian from the input set
    // The outer pair holds diagonal value, and the a vector of the following:
    // Result is Per-vertex [ordered by vertex index] list of adjacent std::pair<vertex, weight>
    std::vector<std::tuple<int, float, std::vector<std::pair<int, float>>>> cotanLaplacian(HalfEdgeMesh mesh) {
        buildVertexToHE(mesh);
        int numVertices = mesh.positions.size();
        std::vector<std::tuple<int, float, std::vector<std::pair<int, float>>>> result;
        result.reserve(numVertices);
        for (int i = 0; i < numVertices; i++) {
            float vertexSum = 1e-8;
            std::vector<std::pair<int, float>> connectionsToVertex;
            for (int he_i : outgoingHalfEdges(mesh, mesh.vertexToHE[i])) {
                HalfEdge he = mesh.halfEdges[he_i];
                float laplacian_v_weight = 0.5f * cotan(mesh, he_i);
                if (he.twin >= 0) {
                   laplacian_v_weight += 0.5f * cotan(mesh, he.twin);
                }
                vertexSum += laplacian_v_weight;
                std::pair<int, float> connection = {he_i, -laplacian_v_weight};
                connectionsToVertex.push_back(connection);
            }
            std::tuple<int, float, std::vector<std::pair<int, float>>> vertexValues = {i, vertexSum, connectionsToVertex};
            result.push_back(vertexValues);
        }
        return result;
    }

    // Builds vector of barycentric dual area of each vertex
    std::vector<float> vertexMasses(HalfEdgeMesh mesh) {
        int numVertices = mesh.positions.size();
        std::vector<float> result(numVertices); 
        result.reserve(numVertices);
        for (int i = 0; i < numVertices; i++) {
            result[i] = barycentricDualArea(mesh, mesh.vertexToHE[i]);
        }
        return result;
    }
}