#ifndef TINYGEO_DIFFGEO
#define TINYGEO_DIFFGEO
#include "HalfEdge.hpp"
#include <vector>
#include <tuple>
namespace tinyGeo {
    std::vector<std::vector<std::pair<int, float>>> cotanLaplacian(HalfEdgeMesh mesh);
    std::vector<float> vertexMasses(HalfEdgeMesh mesh);
}
#endif