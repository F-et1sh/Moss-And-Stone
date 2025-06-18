#version 460
layout (location = 0) in vec2 a_Vertex;

out vec2 i_TexCoords;

uniform mat4 u_Transform;

void main() {
	gl_Position = u_Transform * vec4(a_Vertex, 0.0f, 1.0f);
    i_TexCoords = a_Vertex + vec2(0.5f, 0.5f);
}