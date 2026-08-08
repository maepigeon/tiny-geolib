cmake_minimum_required(VERSION 3.10)
set(ProjectName tiny-geolib)
project(${ProjectName})

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

set(SOURCES
    src/Mesh.cpp
    src/HalfEdge.cpp
    src/Geometry.cpp
    src/DiffGeo.cpp
    src/IntrinsicDelaunay.cpp
)

add_library(${ProjectName} STATIC ${SOURCES})
target_compile_features(${ProjectName} PUBLIC cxx_std_23)
target_link_libraries(${ProjectName} PUBLIC glm::glm)
target_include_directories(${ProjectName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)