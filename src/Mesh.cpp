#include "Mesh.hpp"
namespace tinyGeo {
    void Mesh::computeNormals() {
        for (Vertex& v : vertices) {
            v.normal = glm::vec3(0.0f);
        }
        for (int i = 0; i + 2 < indices.size(); i += 3) {
            glm::vec3 v0 = vertices[indices[i]].position;
            glm::vec3 v1 = vertices[indices[i + 1]].position;
            glm::vec3 v2 = vertices[indices[i + 2]].position;
            glm::vec3 triangleNormal = glm::cross(v1 - v0, v2 - v0);
            vertices[indices[i]].normal += triangleNormal;
            vertices[indices[i + 1]].normal += triangleNormal;
            vertices[indices[i + 2]].normal += triangleNormal;
        }
        for (Vertex& v : vertices) {
            if (glm::length(v.normal) > epsilon) {
                v.normal = glm::normalize(v.normal);
            }
            else {
                v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
            }
        }
    }
}