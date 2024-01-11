#version 330 core

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
        fragColor = texture(textureBuffer, uv);
}