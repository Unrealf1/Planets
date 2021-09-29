#pragma once

struct KeyInputEvent {
    int key;
    int scancode; 
    int action; 
    int mods;
};

struct MouseState {
    int left_button;
    int right_button;
    int middle_button;
};

struct MouseMoveInputEvent {
    double xpos;
    double ypos;
    MouseState mouse;
};

struct MouseScrollInputEvent {
    double yoffset;
    double xoffset;
    MouseState mouse;
};
