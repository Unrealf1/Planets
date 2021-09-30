#pragma once

#include "objects/ISceneObject.hpp"


class IGui : public ISceneObject {
public:
    void draw(const DrawInfo&) override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void init() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(_window, false);
        ImGui_ImplOpenGL3_Init("#version 430");
    }
};