#version 410 core

in vec4 clipSpaceCoords;
in vec2 fTexCoords;
in vec3 toCameraVector;
in vec3 fromLightVector;
in vec4 fPosEye;

out vec4 fColor;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float moveFactor;
float waveStrength = 0.002f;

uniform vec3 lightColor;
float shininess = 20.0f;
float reflectivity = 0.5f;

float computeFog()
{
	float fogDensity = 0.035f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

void main()
{
	vec2 ndc = (clipSpaceCoords.xy / clipSpaceCoords.w) / 2.0f + 0.5f;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractTexCoords = vec2(ndc.x, ndc.y);

	float near = 0.1f;
	float far = 200.0f;
	float depth = texture(depthMap, refractTexCoords).r;
	float floorDistance = 2.0f * near * far / (far + near - (2.0f * depth - 1.0f) * (far - near));

	depth = gl_FragCoord.z;
	float waterDistance = 2.0f * near * far / (far + near - (2.0f * depth - 1.0f) * (far - near));
	float waterDepth = floorDistance - waterDistance;

	//vec2 distortion1 = texture(dudvMap, vec2(fTexCoords.x + moveFactor, fTexCoords.y)).rg * 2.0f - 1.0f;
    //vec2 distortion2 = texture(dudvMap, vec2(-fTexCoords.x + moveFactor, fTexCoords.y + moveFactor)).rg * 2.0f - 1.0f;
    //vec2 distortion = (distortion1 + distortion2) * waveStrength;

	vec2 distortedTexCoords = texture(dudvMap, vec2(fTexCoords.x + moveFactor, fTexCoords.y)).rg * 0.1f;
	distortedTexCoords = fTexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 distortion = texture(dudvMap, distortedTexCoords).rg * 2.0f - 1.0f;
	distortion *= waveStrength;

	reflectTexCoords += distortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001f, 0.999f);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999f, -0.001f);

	refractTexCoords += distortion;
	refractTexCoords = clamp(refractTexCoords, 0.001f, 0.999f);

	vec4 reflectionColor = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColor = texture(refractionTexture, refractTexCoords);

	vec3 viewVector = normalize(toCameraVector);
	float refractiveFactor = dot(viewVector, vec3(0.0f, 1.0f, 0.0f));
	refractiveFactor = pow(refractiveFactor, 2.0f);

	vec4 normalMapColor = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalMapColor.r * 2.0f - 1.0f, normalMapColor.b, normalMapColor.g * 2.0f - 1.0f);
	normal = normalize(normal);

	vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0f);
	specular = pow(specular, shininess);
	vec3 specularComponent = lightColor * specular * reflectivity;

	float murkyWaterFactor = clamp(waterDepth, 0.0f, 1.0f);
	vec4 murkyWaterColor = vec4(0.2f, 0.1f, 0.0f, 1.0f);
	refractionColor = refractionColor * (1.0f - murkyWaterFactor) + murkyWaterColor * murkyWaterFactor;

	fColor = reflectionColor * (1 - refractiveFactor) + refractionColor * refractiveFactor;
	fColor = fColor * 0.8f + vec4(0.0f, 0.05f, 0.15f, 1.0f) * 0.2f;
	fColor += vec4(specularComponent, 0.0f);
	fColor.a = clamp(waterDepth / 0.1f, 0.0f, 1.0f);

    float fogFactor = computeFog();
	vec4 fogColor = vec4(0.2f, 0.2f, 0.4f, 1.0f);
	fColor = fogColor * (1 - fogFactor) + fColor * fogFactor;
}