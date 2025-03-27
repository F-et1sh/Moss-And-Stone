#version 460
layout (location = 0) in vec2 aVertex;

out vec2 iTexCoords;

uniform mat4 uTransform;

void main() {
	gl_Position = uTransform * vec4(aVertex - vec2(0.5f, 0.5f), 0.0f, 1.0f);
    iTexCoords = aVertex;
    iTexCoords.y = 1 - aVertex.y;
}