#version 460
layout (location = 0) in vec2 aVertex;

out vec2 i_TexCoords;

layout (std140, binding = 1) uniform uSpriteData {
    vec4 uAtlasSize;

    // Sprites Limit at One Drawcall
    #define SPRITES_LIMIT 256

    // Sprite Transforms as mat4
    mat4 u_Transforms[SPRITES_LIMIT];

    // Atlas Offset - XY | Texture Size - ZW
    vec4 u_TexCoords[SPRITES_LIMIT];
}data;

uniform mat4 u_ViewProj;

void main() {
	gl_Position = u_ViewProj * data.u_Transforms[gl_InstanceID] * vec4(aVertex, 0.0f, 1.0f);
	i_TexCoords = ((aVertex + vec2(0.5f, 0.5f))
                * vec2(data.u_TexCoords[gl_InstanceID].z, data.u_TexCoords[gl_InstanceID].w)
                + data.u_TexCoords[gl_InstanceID].xy)
                / data.uAtlasSize.xy;
}