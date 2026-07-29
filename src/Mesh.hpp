#ifndef TINYGEO_MESH
#define TINYGEO_MESH
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
namespace tinyGeo {
    const double epsilon = 1e-12f;
    struct Vertex {
        glm::vec3 position {0.0f};
        glm::vec3 normal {0.0f, 0.0f, 1.0f};
        glm::vec2 uv {0.0f};
    };
    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        int triangleCount() { 
            return indices.size() / 3;
        }
        void clear() { 
            vertices.clear();
            indices.clear();
        }
        void computeNormals();
    };
} 
#endif