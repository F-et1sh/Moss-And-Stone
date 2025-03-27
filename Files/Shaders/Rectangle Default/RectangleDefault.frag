#version 460
out vec4 fragColor;

in vec2 iTexCoords;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uIsTextured;

void main() {
	if (uIsTextured) fragColor = texture2D(uTexture, iTexCoords) * uColor;
	else			 fragColor = uColor;
}