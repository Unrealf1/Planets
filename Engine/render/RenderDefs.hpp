#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

// TODO: should this really be here?
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>

/*
My vector space:
X to the right
Y from the screen
Z up
*/

constexpr std::size_t instance_render_limit = 100;

template <typename Point = glm::vec3>
using Mesh = std::vector<Point>;

template<typename Point=glm::vec3>
struct Triangle {
    Point a;
    Point b;
    Point c;
};


template<typename Point>
inline Mesh<Point> FlattenTriangles(std::vector<Triangle<Point>> triangles) {
    Mesh<Point> result;
    result.reserve(triangles.size() * 3);

    for (const auto& triangle : triangles) {
        result.push_back(triangle.a);
        result.push_back(triangle.b);
        result.push_back(triangle.c);
    }
    
    return result;
}

template<typename Point=glm::vec3>
struct Model {
    Mesh<Point> mesh;
    Mesh<Point> norms;
};

template<typename Point=glm::vec3, typename TexPoint = glm::vec2>
struct TexturedModel: Model<Point>{
    Mesh<TexPoint> texCoords;
};


struct GraphicObject {
    GLuint vbo_coords = 0;
    GLuint vbo_norms = 0;
    GLuint vbo_texcoords = 0;
    GLuint vao = 0;
    GLsizei vertex_cnt = 0;
    GLuint texture = 0;
};
