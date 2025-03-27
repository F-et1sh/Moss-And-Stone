#version 460
layout (location = 0) in vec2 aVertexPos;

out vec2 iTexCoords;
out vec4 iTextColor;

layout (std140, binding = 0) uniform uTextData {
    vec4 uAtlasSize;
    vec4 uTextColor;

    // Letters Limit at One Drawcall
    #define LETTERS_LIMIT 256

    mat4 uTransforms[LETTERS_LIMIT];

    // XY - Atlas Offset | ZW - Glyph Size
    vec4 uAtlasOffsets[LETTERS_LIMIT];
}data;

uniform mat4 uCamera;

void main() {
    // Setting Vertex Position
    gl_Position = uCamera * data.uTransforms[gl_InstanceID] * vec4(aVertexPos, 0.0f, 1.0f);

    // Setting Texture Coordinates
    iTexCoords = aVertexPos.xy + vec2(0.5f, 0.5f);
    iTexCoords.y = 1.0f - iTexCoords.y;
    
    iTexCoords *= data.uAtlasOffsets[gl_InstanceID].zw;
    iTexCoords += data.uAtlasOffsets[gl_InstanceID].xy;
    iTexCoords /= data.uAtlasSize.xy;
    
    // Setting Text Color
    iTextColor = data.uTextColor;
}