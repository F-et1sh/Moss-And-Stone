#version 460
layout (location = 0) in vec2 a_Vertex;

out vec2 i_TexCoords;

layout (std140, binding = 2) uniform uTileeData {
    // tiles counts at one drawcall
    #define TILES_LIMIT 256

    // XY - position
    // ZW - atlas offset
    vec4 u_TileInfo[TILES_LIMIT];

    mat4 u_TilemapTransform;
}data;

uniform mat4 u_ViewProj;

void main() {
	gl_Position = u_ViewProj * data.u_TilemapTransform * vec4(a_Vertex + data.u_TileInfo[gl_InstanceID].xy, 0.0f, 1.0f);
	i_TexCoords = a_Vertex + data.u_TileInfo[gl_InstanceID].zw;
}