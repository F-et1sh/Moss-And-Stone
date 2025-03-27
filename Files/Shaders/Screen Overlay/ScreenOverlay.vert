#version 460
layout (location = 0) in vec2 aVertex;

out vec2 iTexCoords;

layout (std140, binding = 0) uniform uSpriteData {
    vec4 uAtlasSize;

    // Sprites Limit at One Drawcall
    #define SPRITES_LIMIT 256

    // Sprite Transforms as mat4
    mat4 uTransforms[SPRITES_LIMIT];

    // Atlas Offset - XY | Texture Size - ZW
    vec4 uTexCoords[SPRITES_LIMIT];
}data;

uniform mat4 uCamera;

void main() {
	gl_Position = uCamera * data.uTransforms[gl_InstanceID] * vec4(aVertex, 0.0f, 1.0f);
	iTexCoords = ((vec2(data.uTexCoords[gl_InstanceID].z, -data.uTexCoords[gl_InstanceID].w) * (aVertex + vec2(0.5f, -0.5f))) + data.uTexCoords[gl_InstanceID].xy) / data.uAtlasSize.xy;
}