#ifndef TINYGEO_DIFFGEO
#define TINYGEO_DIFFGEO
#include "HalfEdge.hpp"
#include <vector>
#include <tuple>
namespace tinyGeo {
    std::vector<std::tuple<int, float, std::vector<std::pair<int, float>>>> cotanLaplacian(HalfEdgeMesh mesh);
    std::vector<float> vertexMasses(HalfEdgeMesh mesh);
}
#endif