cmake_minimum_required(VERSION 3.20) #cpp23 requires cmake v3.20 or higher
project(tiny-geolib)
# Use the C++23 standard.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include(FetchContent)

find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
    message(STATUS "glm not found on system, fetching from source...")
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(glm)
endif()
add_library(tiny-geolib STATIC
    src/tinyGeo/Mesh.cpp
    src/tinyGeo/HalfEdge.cpp
    src/tinyGeo/Geometry.cpp
    src/tinyGeo/DiffGeo.cpp
    src/tinyGeo/IntrinsicDelaunay.cpp
)
target_compile_features(tiny-geolib PUBLIC cxx_std_23)
target_link_libraries(tiny-geolib PUBLIC glm::glm)
target_include_directories(
    tiny-geolib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)