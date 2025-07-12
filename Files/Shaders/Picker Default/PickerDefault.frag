#version 460
out int entityHandle;

in vec4 i_EntityHandle;

in vec2 i_TexCoords;
uniform sampler2D u_Texture;

void main() {
    vec4 frag_color = texture2D(u_Texture, i_TexCoords);
    if (frag_color.a == 0) discard;

    entityHandle = int(i_EntityHandle.x);
}