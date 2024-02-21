#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;
in vec4 fragPosLightSpace;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightPos3;
uniform vec3 lightPos4;
uniform vec3 lightPos5;
uniform vec3 lightPos6;
uniform vec3 lightPos7;
uniform vec3 lightPos8;
uniform vec3 lightPosColor;

uniform vec3 flashlightPos;
uniform vec3 flashlightDir;
uniform float cutOff;
uniform float outerCutOff;
uniform int on;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

float shadow;

// components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

vec3 ambientPoint[8];
float ambientPointStrength = 0.05f;
vec3 diffusePoint[8];
float diffusePointStrength = 0.8f;
vec3 specularPoint[8];
float specularPointStrength = 1.0f;

vec3 diffuseFlashlight;
vec3 specularFlashlight;

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(-fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

void computePointLight(vec3 lightPos, int i)
{
    //compute eye space coordinates
    //vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 fNormalN = normalize(fNormal);

    //compute view direction (in eye coordinates, the viewer is situated at the origin)
    vec3 viewDir = normalize(-fPosition);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(lightPos - fPosition));

    float distance = length(lightPos - fPosition);
    float attenuation = 1.0 / (1.0 + 0.22 * distance + 0.2 * (distance * distance));

    //compute ambient light
    ambientPoint[i] = ambientPointStrength * lightPosColor * attenuation;

    //compute diffuse light
    diffusePoint[i] = diffusePointStrength * max(dot(fNormalN, lightDirN), 0.0f) * lightPosColor * attenuation;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, fNormalN);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specularPoint[i] = specularPointStrength * specCoeff * lightPosColor * attenuation;
}

void computeFlashLight()
{
    vec3 lightDirN = vec3(normalize(flashlightPos - fPosition));
    float distance = length(flashlightPos - fPosition);
    float attenuation = 1.0 / (1.0 + 0.14 * distance + 0.07 * (distance * distance));

    float theta = dot(lightDirN, normalize(-flashlightDir));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    vec3 whiteColor = vec3(1.0f, 1.0f, 1.0f);

    diffuseFlashlight = whiteColor * intensity * attenuation;
    specularFlashlight = whiteColor * intensity * attenuation;
}

float computeFog()
{
	float fogDensity = 0.03f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

void computeShadow()
{
	// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;
	if (normalizedCoords.z > 1.0f)
	{
		shadow = 0.0f;
		return;
	}

    float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
    shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(float x = -1; x <= 1; x += 0.5)
    {
        for(float y = -1; y <= 1; y += 0.5)
        {
            float pcfDepth = texture(shadowMap, normalizedCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += (currentDepth - bias > pcfDepth ? 1.0 : 0.0);        
        }    
    }
    shadow /= 25.0;

	//// Check whether current frag pos is in shadow
	//shadow = currentDepth > closestDepth ? 1.0 : 0.0;

	// Check whether current frag pos is in shadow
	//float bias = 0.005f;
    //float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
	//shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
}

void main() 
{
    computeDirLight();
    computePointLight(lightPos1, 0);
    computePointLight(lightPos2, 1);
    computePointLight(lightPos3, 2);
    computePointLight(lightPos4, 3);
    computePointLight(lightPos5, 4);
    computePointLight(lightPos6, 5);
    computePointLight(lightPos7, 6);
    computePointLight(lightPos8, 7);
    computeFlashLight();
    computeShadow();

    //compute final vertex color
    //vec3 color = min((ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb + (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
    //fColor = vec4(color, 1.0f);
    vec3 color = (ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb + (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb;
    int i;
    for (i = 0; i < 8; ++i)
    {
        color += ((ambientPoint[i] + diffusePoint[i]) * texture(diffuseTexture, fTexCoords).rgb + specularPoint[i] * texture(specularTexture, fTexCoords).rgb);
    }
    if (on == 1)
    {
        color += (diffuseFlashlight * texture(diffuseTexture, fTexCoords).rgb + specularFlashlight * texture(specularTexture, fTexCoords).rgb);
    }

    vec4 colorr = vec4(color, 1.0f);
    float fogFactor = computeFog();
	vec4 fogColor = vec4(0.2f, 0.2f, 0.4f, 1.0f);
	fColor = fogColor * (1 - fogFactor) + colorr * fogFactor;
}
