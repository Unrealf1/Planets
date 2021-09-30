#pragma once

#include "objects/primitives/IGui.hpp"

class GameGui : public IGui {
public:
    GameGui(GLFWwindow* window) : IGui(window) {
        
    }

    void update(const UpdateInfo& info) override {
        _player_pos = info.player_position;
    }

    bool isMarkedForDeletion() override { return false; }
    void markForDeletion() override {}

    void actualDraw(const DrawInfo&) override {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("GUI", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("FPS %.1f", static_cast<double>(ImGui::GetIO().Framerate));
            ImGui::Text(
                "Player position: %.1f, %.1f, %.1f", 
                static_cast<double>(_player_pos.x), 
                static_cast<double>(_player_pos.y), 
                static_cast<double>(_player_pos.z)
            );
        }
        ImGui::End();
    }
private:
    glm::vec3 _player_pos;
};
