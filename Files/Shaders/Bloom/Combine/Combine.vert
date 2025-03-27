#version 460
layout (location = 0) in vec2 aVertex;

out vec2 iTexCoords;

void main() {
	gl_Position = vec4(aVertex, 0.0f, 0.5f);
	iTexCoords = aVertex + vec2(0.5f, 0.5f);
}