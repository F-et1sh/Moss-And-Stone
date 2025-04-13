#version 460
layout (location = 0) in vec2 a_Vertex;

out vec4 i_EntityHandle;

layout (std140, binding = 1) uniform u_EntityData {
    // entity limit at one drawcall
    #define ENTITY_LIMIT 256

    mat4 u_Transforms[ENTITY_LIMIT];
    vec4 u_EntityHandles[ENTITY_LIMIT];
}data;

uniform mat4 u_Camera;

void main() {
	gl_Position = u_Camera * data.u_Transforms[gl_InstanceID] * vec4(a_Vertex, 0.0f, 1.0f);
	i_EntityHandle = data.u_EntityHandles[gl_InstanceID];
}