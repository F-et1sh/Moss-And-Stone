#version 460
layout (location = 0) in vec2 aVertex;
layout (location = 1) in vec2 aTexCoords;

out vec2 iTexCoords;

layout (std140, binding = 0) uniform uMeshData {
    mat4 uTransform;
}data;

void main() {
	gl_Position = data.uTransform * vec4(aVertex, 0.0f, 1.0f);
	iTexCoords = aTexCoords;
}