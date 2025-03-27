#version 460
out vec4 fragColor;
in vec2 iTexCoords;

uniform sampler2D uTexture;
uniform vec4 uTheshold;

void main() {
    fragColor = texture2D(uTexture, iTexCoords);
    fragColor *= vec4(dot(fragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f)))/uTheshold;
}