#include "objects/primitives/DrawableModel.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


void DrawableModel::draw(const DrawInfo& info) {
    auto program = ProgramContainer::getProgram(program_name);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _graphics.texture);

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
    auto model = glm::mat4(1.0f);
    auto translation = glm::translate(model, _position);
    auto scale = glm::scale(model, _scale);
    auto rotation = glm::toMat4(_rotation);
    model = translation * scale * rotation;
    glUniformMatrix4fv(
        glGetUniformLocation(program, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );
    glUniform1f(
        glGetUniformLocation(program, "seconds"),
        info.seconds_since_start
    );
    glDrawArrays(GL_TRIANGLES, 0, _graphics.vertex_cnt);
    glBindVertexArray(0);
}