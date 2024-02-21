#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=2) in vec2 vTexCoords;

out vec4 clipSpaceCoords;
out vec2 fTexCoords;
out vec3 toCameraVector;
out vec3 fromLightVector;
out vec4 fPosEye;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;

void main()
{
	vec4 worldPosition = model * vec4(vPosition, 1.0f);
	clipSpaceCoords = projection * view * worldPosition;
	fTexCoords = vTexCoords;
	toCameraVector = cameraPosition - worldPosition.xyz;
	fromLightVector = worldPosition.xyz - lightPosition;
	fPosEye = view * worldPosition;
	gl_Position = clipSpaceCoords;
}