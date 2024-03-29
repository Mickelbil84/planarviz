#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uvCoord;

struct Camera {
    vec2 position;
    float zoom, aspectRatio;
};

uniform vec2 position;
uniform float rotation;
uniform float scale;
uniform float depth;
uniform Camera camera;

out vec2 uv;

void main() {
    uv = vec2(uvCoord.xy);
    float ct = cos(rotation), st = sin(rotation);
    vec2 finalPos = pos * scale;
    finalPos = (vec2(finalPos.x * ct - finalPos.y * st, finalPos.x * st + finalPos.y * ct) + position) * camera.zoom + camera.position;
    finalPos.y *= camera.aspectRatio;
    gl_Position = vec4(finalPos, depth, 1.0);
}