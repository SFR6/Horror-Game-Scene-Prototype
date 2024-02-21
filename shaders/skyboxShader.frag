#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;

void main()
{
    vec4 colorr = texture(skybox, textureCoordinates);
    vec4 fogColor = vec4(0.2f, 0.2f, 0.4f, 1.0f);
    color = fogColor * 0.5 + colorr * 0.5;
}
