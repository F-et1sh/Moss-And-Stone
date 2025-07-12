#version 460
layout (location = 0) in vec2 a_Vertex;

out vec4 i_EntityHandle;
out vec2 i_TexCoords;

layout (std140, binding = 0) uniform u_EntityData {
    vec4 u_AtlasSize;

    // entity limit at one drawcall
    #define ENTITY_LIMIT 256

    mat4 u_Transforms[ENTITY_LIMIT];
    vec4 u_TexCoords[ENTITY_LIMIT];
    vec4 u_EntityHandles[ENTITY_LIMIT];
}data;

uniform mat4 u_ViewProj;

void main() {
	gl_Position = u_ViewProj * data.u_Transforms[gl_InstanceID] * vec4(a_Vertex, 0.0f, 1.0f);
	i_EntityHandle = data.u_EntityHandles[gl_InstanceID];
	i_TexCoords = ((a_Vertex + vec2(0.5f, 0.5f))
                * vec2(data.u_TexCoords[gl_InstanceID].z, data.u_TexCoords[gl_InstanceID].w)
                + data.u_TexCoords[gl_InstanceID].xy)
                / data.u_AtlasSize.xy;

}