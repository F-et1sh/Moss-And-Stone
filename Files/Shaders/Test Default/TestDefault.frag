#version 460
out vec4 fragColor;

in vec2 iTexCoords;

uniform sampler2D uTexture;

void main() {
    fragColor = texture2D(uTexture, iTexCoords);
    if (fragColor.a == 0) discard;
}