#include "objects/primitives/Sphere.hpp"

#include <numeric>
#include <iostream>


std::vector<Triangle<>> Sphere::getIcosahedronTriangles() {
    static bool initialized = false;
    static std::vector<Triangle<>> triangles;
    if (initialized) {
        return triangles;
    }
    triangles.reserve(20);

    std::array<glm::vec3, 12> base_points;
    // create base of a sphere (20 triangles of the icosahedron)
    float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
    
    base_points[0] = {-1,  t,  0}; 
    base_points[1] = { 1,  t,  0};
    base_points[2] = {-1, -t,  0};
    base_points[3] = { 1, -t,  0};

    base_points[4] = { 0, -1,  t}; // 4
    base_points[5] = { 0,  1,  t}; // 5
    base_points[6] = { 0, -1, -t}; // 6
    base_points[7] = { 0,  1, -t}; // 7

    base_points[8] = { t,  0, -1}; // 8
    base_points[9] = { t,  0,  1}; // 9
    base_points[10] = {-t,  0, -1}; // 10
    base_points[11] = {-t,  0,  1}; // 11

    std::array<size_t, 60> base_indexes = {
        0, 11, 5,
        0, 5, 1,
        0, 1, 7,
        0, 7, 10,
        0, 10, 11,

        1, 5, 9,
        5, 11, 4,
        11, 10, 2,
        10, 7, 6,
        7, 1, 8,

        3, 9, 4,
        3, 4, 2,
        3, 2, 6,
        3, 6, 8,
        3, 8, 9,

        4, 9, 5,
        2, 4, 11,
        6, 2, 10,
        8, 6, 7,
        9, 8, 1
    };

    for (size_t i = 0; i < base_indexes.size(); i +=3) {
        triangles.push_back({
            glm::normalize(base_points[base_indexes[i]]),
            glm::normalize(base_points[base_indexes[i + 1]]),
            glm::normalize(base_points[base_indexes[i + 2]])
        });
    }

    initialized = true;
    return triangles;
}

std::vector<Triangle<>> Sphere::refineSphere(const std::vector<Triangle<>>& current_sphere) {
    std::vector<Triangle<>> refined;
    refined.reserve(current_sphere.size() * 4);

    for (const auto& triangle : current_sphere) {
        auto a = (triangle.a + triangle.b) / 2.0f;
        auto b = (triangle.b + triangle.c) / 2.0f;
        auto c = (triangle.c + triangle.a) / 2.0f;
        
        refined.push_back({ a, b, c });
        refined.push_back({ triangle.a, a, c });
        refined.push_back({ triangle.b, b, a });
        refined.push_back({ triangle.c, c, b });
    }

    for (auto& triangle : refined) {
        triangle.a = glm::normalize(triangle.a);
        triangle.b = glm::normalize(triangle.b);
        triangle.c = glm::normalize(triangle.c);
    }

    return refined;
}

void Sphere::draw(const DrawInfo& info) {
    std::cout << "getting program..." << std::endl;
    auto program = ProgramContainer::getProgram("simple prog");

    std::cout << "got program, it's id is " << program << std::endl;
    glUseProgram(program);
    glBindVertexArray(_graphics.vao);
    glUniformMatrix4fv(
        glGetUniformLocation(program, "view"),
        1,
        GL_FALSE,
        glm::value_ptr(info.view_mat)
    );
    glUniformMatrix4fv(
        glGetUniformLocation(program, "projection"),
        1,
        GL_FALSE,
        glm::value_ptr(info.proj_mat)
    );
    auto model = glm::vec4(1.0f);
    glUniformMatrix4fv(
        glGetUniformLocation(program, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );
    std::cout << "almost draw..." << std::endl;
    glDrawArrays(GL_TRIANGLES, 0, _graphics.vertex_cnt);
    glBindVertexArray(0);
}
