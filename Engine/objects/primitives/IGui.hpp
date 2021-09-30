#pragma once

#include "objects/ISceneObject.hpp"
#include <GLFW/glfw3.h>

/// Imgui is demanding. It did cost me about 5k fps :)
class IGui : public ISceneObject {
public:
    IGui(GLFWwindow* window) : _window(window) {

    }

    virtual ~IGui() = default;

    void draw(const DrawInfo& info) override {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        actualDraw(info);
    }

    void init() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(_window, false);
        ImGui_ImplOpenGL3_Init("#version 430");

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

protected:
    GLFWwindow* _window;

    virtual void actualDraw(const DrawInfo&) = 0;
};