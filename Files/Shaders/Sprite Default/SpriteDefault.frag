#version 460
out vec4 fragColor;

in vec2 i_TexCoords;

uniform sampler2D u_Texture;

void main() {
    fragColor = texture2D(u_Texture, i_TexCoords);
    if (fragColor.a == 0) discard;
}