#version 430

layout(location = 0) in vec3 vertexPosition;

out vec3 colour;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform float seconds;

void main() {
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    colour = vec3((sin(seconds*1.7) + 1.0) / 2.0, (cos(seconds) + 1.0) / 2.0, (sin(seconds) + 1.0) / 2.0);
}
