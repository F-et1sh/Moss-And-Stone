#version 460
out int entityHandle;

in vec4 i_EntityHandle;

void main() {
    entityHandle = int(i_EntityHandle.x);
}