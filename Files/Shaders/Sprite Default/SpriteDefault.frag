#version 460
out vec4 fragColor;

in vec2 iTexCoords;

uniform sampler2D u_Texture;

void main() {
    fragColor = texture2D(u_Texture, iTexCoords);
    if (fragColor.a == 0) discard;
}