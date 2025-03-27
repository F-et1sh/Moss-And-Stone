#version 460
out vec4 fragColor;
  
in vec2 iTexCoords;

uniform sampler2D uTexture;
  
uniform bool uIsHorizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform int uQueueNum;

void main() {
    vec2 texCoords = iTexCoords * uQueueNum;
    vec2 tex_offset = 1.0 / textureSize(uTexture, 0);
    vec3 result = texture(uTexture, texCoords).rgb * weight[0];
    if (uIsHorizontal) {
        for (int i = 1; i < 5; i++) {
            result += texture2D(uTexture, texCoords + vec2(tex_offset.x * i, 0.0f)).rgb * weight[i];
            result += texture2D(uTexture, texCoords - vec2(tex_offset.x * i, 0.0f)).rgb * weight[i];
        }
    }
    else {
        for (int i = 1; i < 5; i++) {
            result += texture2D(uTexture, texCoords + vec2(0.0f, tex_offset.y * i)).rgb * weight[i];
            result += texture2D(uTexture, texCoords - vec2(0.0f, tex_offset.y * i)).rgb * weight[i];
        }
    }
    fragColor = vec4(result, 1.0f);
}