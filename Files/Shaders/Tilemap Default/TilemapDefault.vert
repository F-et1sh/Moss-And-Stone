#version 460
layout (location = 0) in vec2 a_Vertex;

out vec2 i_TexCoords;

layout (std140, binding = 2) uniform uTileeData {
    // tiles counts at one drawcall
    #define TILES_LIMIT 256

    vec4 u_AtlasSize;

    vec4 u_TileFrame[TILES_LIMIT];
    mat4 u_TileTransform[TILES_LIMIT];

    mat4 u_TilemapTransform;
}data;

uniform mat4 u_ViewProj;

void main() {
	gl_Position = u_ViewProj * data.u_TilemapTransform * data.u_TileTransform[gl_InstanceID] * vec4(a_Vertex, 0.0f, 1.0f);
	i_TexCoords = (a_Vertex + vec2(0.5f)) * data.u_TileFrame[gl_InstanceID].zw + data.u_TileFrame[gl_InstanceID].xy;
	i_TexCoords /= data.u_AtlasSize.xy;

}