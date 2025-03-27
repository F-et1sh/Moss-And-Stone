#version 460
layout (location = 0) out vec4 fragColor;

in vec2 iTexCoords;
in vec4 iTextColor;

uniform sampler2D uTexture;

void main() {
    if (texture2D(uTexture, iTexCoords).r == 1.0f) {
        fragColor = iTextColor;
        //fragColor.rgb = mix(fragColor.rgb, vec3(0.5f), -1.5f);
    }
    else discard;
    //else fragColor = vec4(1.0f, 1.0f, 1.0f, 0.0f);
    //fragColor.a += 0.25f;
}