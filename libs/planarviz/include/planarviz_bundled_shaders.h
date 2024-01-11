// Auto Generated File: DO NOT EDIT!
#pragma once
namespace planarviz {
	constexpr const char* default_fs = R"V0G0N(#version 330 core

in vec2 uv;

out vec4 fragColor;

uniform vec4 geometryColor;

uniform vec4 hoverColor;
uniform bool isHovered;

uniform bool useTexture;
uniform sampler2D textureBuffer;

void main() {
    fragColor = geometryColor;
    if (isHovered)
        fragColor = hoverColor;
    if (useTexture)
        fragColor = vec4(texture(textureBuffer, uv).rgb, 1);
})V0G0N";

	constexpr const char* default_vs = R"V0G0N(#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uvCoord;

struct Camera {
    vec2 position;
    float zoom, aspectRatio;
};

uniform vec2 position;
uniform float rotation;
uniform float depth;
uniform Camera camera;

out vec2 uv;

void main() {
    uv = vec2(uvCoord.xy);
    float ct = cos(rotation), st = sin(rotation);
    vec2 finalPos = (vec2(pos.x * ct - pos.y * st, pos.x * st + pos.y * ct) + position) * camera.zoom + camera.position;
    finalPos.y *= camera.aspectRatio;
    gl_Position = vec4(finalPos, depth, 1.0);
})V0G0N";


	#define DEFAULT_VERTEX_SHADER default_vs
	#define DEFAULT_FRAGMENT_SHADER default_fs
}