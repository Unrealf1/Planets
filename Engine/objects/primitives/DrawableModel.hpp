#pragma once

#include "render/RenderDefs.hpp"
#include "objects/ISceneObject.hpp"
#include "render/util/GraphicsInitializer.hpp"
#include "render/util/TextureContainer.hpp"
#include "shader/ProgramContainer.hpp"
#include "shader/Program.hpp"

#include <filesystem>
#include <spdlog/spdlog.h>


class DrawableModel : public IBasicSceneObject {
public:
    DrawableModel(TexturedModel<> model, std::filesystem::path texture_path)
    :
    _model(std::move(model)),
    _texture_path(texture_path)
    {

    }

    void draw(const DrawInfo& info) override;

    void init() override {
        _graphics = GraphicsInitializer::initObject(_model, _texture_path);

        if (ProgramContainer::checkProgram(program_name)) {
            return;
        }

        spdlog::debug("DrawableModel: creating program ...");

        auto shaders_dir = std::filesystem::path("resources/shaders");
        shaders_dir.make_preferred();

        auto text = extractShaderText(shaders_dir / "Textured.vert");
        auto vertex_shader = createVertexShader(text.c_str());

        text = extractShaderText(shaders_dir / "Textured.frag");
        auto frag_shader = createFragmentShader(text.c_str());

        auto program = createProgram(vertex_shader, frag_shader);
        ProgramContainer::registerProgram(program_name, program);
    }

    void update(const UpdateInfo&) override { }

    glm::vec3 getPosition() const override {
        return _position;
    }

    glm::quat getRotation() const override {
        return _rotation;
    }

    glm::vec3 getScale() const override {
        return _scale;
    }

    void setPosition(const glm::vec3& position) override {
        _position = position;
    }

    void setRotation(const glm::quat& rotation) override {
        _rotation = rotation;
    }

    void setScale(const glm::vec3& scale) override {
        _scale = scale;
    }

private:
    glm::vec3 _position;
    glm::quat _rotation;
    glm::vec3 _scale;
    TexturedModel<> _model;
    std::filesystem::path _texture_path;
    GraphicObject _graphics;

    static constexpr char const* program_name = "textured";

};