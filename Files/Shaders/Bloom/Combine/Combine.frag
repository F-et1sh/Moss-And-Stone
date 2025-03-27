#version 460
out vec4 fragColor;

in vec2 iTexCoords;

uniform sampler2D uTexture_0;
uniform sampler2D uTexture_1;
uniform sampler2D uTexture_2;
uniform sampler2D uTexture_3;
uniform sampler2D uTexture_4;
uniform sampler2D uTexture_5;

uniform sampler2D uSourceTexture;

void main() {
    fragColor = texture2D(uSourceTexture, iTexCoords);
    fragColor += (texture2D(uTexture_0, iTexCoords)+
                  texture2D(uTexture_1, iTexCoords)+
                  texture2D(uTexture_2, iTexCoords)+
                  texture2D(uTexture_3, iTexCoords)+
                  texture2D(uTexture_4, iTexCoords)+
                  texture2D(uTexture_5, iTexCoords));
    fragColor /= vec4(vec3(6), 1.0f);
}