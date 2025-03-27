#version 460
layout (location = 0) in vec2 aVertex;

layout (std140, binding = 0) uniform uSpriteData {
    vec4 uAtlasSize;

    // Sprites Limit at One Drawcall
    #define SPRITES_LIMIT 256

    // Sprite Transforms as mat4
    mat4 uTransforms[SPRITES_LIMIT];

    // Atlas Offset - XY | Texture Size - ZW
    vec4 uTexCoords[SPRITES_LIMIT];
}data;

void main() {
    // TODO : Camera
	gl_Position = data.uTransforms[gl_InstanceID] * vec4(aVertex, 0.0f, 2048.0f);
}