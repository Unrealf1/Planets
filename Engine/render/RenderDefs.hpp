#pragma once

#include <cstddef>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

// TODO: should this really be here?
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


/*
My vector space:
X to the right
Y from the screen
Z up
*/

constexpr std::size_t instance_render_limit = 100;
using Point = glm::vec3;

using Mesh = std::vector<Point>;

struct Triangle {
    Point a;
    Point b;
    Point c;
};

inline Mesh FlattenTriangles(std::vector<Triangle> triangles) {
    return {};
}

struct Model {
    Mesh<Point> mesh;
    Mesh<Point> norms;
};

template<typename TexPoint = glm::vec2>
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
