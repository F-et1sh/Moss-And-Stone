#version 460
out vec4 fragColor;

in vec2 iTexCoords;

uniform sampler2D uTexture;
uniform float u_OverlayAlpha;

void main() {
    fragColor = vec4(0.0f, 0.0f, 0.0f, u_OverlayAlpha);
}