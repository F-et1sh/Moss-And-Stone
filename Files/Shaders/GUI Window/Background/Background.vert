#version 460
layout (location = 0) in vec2 aVertex;

layout (std140, binding = 0) uniform uBackgroundData {
	mat4 uTransform;
}data;

void main() {
	gl_Position = data.uTransform * vec4(aVertex, 0.0f, 1.0f);
}