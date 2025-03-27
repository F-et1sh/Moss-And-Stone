#version 460
out vec4 fragColor;

in vec2 iTexCoords;

uniform sampler2D uTexture;

vec3 ACESFilm(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    fragColor = texture2D(uTexture, iTexCoords);
    fragColor.rgb = ACESFilm(fragColor.rgb);
}