#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 lightPos;
uniform vec3 lightPosColor;

uniform vec3 flashlightPos;
uniform vec3 flashlightDir;
uniform float cutOff;
uniform float outerCutOff;
uniform int on;

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

vec3 ambientPoint;
float ambientPointStrength = 0.05f;
vec3 diffusePoint;
float diffusePointStrength = 0.8f;
vec3 specularPoint;
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

void computePointLight()
{
    //compute eye space coordinates
    //vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 fNormalN = normalize(fNormal);

    //compute view direction (in eye coordinates, the viewer is situated at the origin)
    vec3 viewDir = normalize(-fPosition);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(lightPos - fPosition));

    float distance = length(lightPos - fPosition);
    float attenuation = 1.0 / (1.0 + 5 * 0.09 * distance + 5 * 0.032 * (distance * distance));
    if (fPosition.y > 0.5)
    {
        attenuation = 1.0 / (1.0 + 0.35 * distance + 3 * 0.44 * (distance * distance));
    }

    //compute ambient light
    ambientPoint = ambientPointStrength * lightPosColor * attenuation;

    //compute diffuse light
    diffusePoint = diffusePointStrength * max(dot(fNormalN, lightDirN), 0.0f) * lightPosColor * attenuation;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, fNormalN);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specularPoint = specularPointStrength * specCoeff * lightPosColor * attenuation;
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

void main() 
{
    computeDirLight();
    computePointLight();
    computeFlashLight();

    //compute final vertex color
    //vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
    vec3 color = (ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb;
    color += ((ambientPoint + diffusePoint) * texture(diffuseTexture, fTexCoords).rgb + specularPoint * texture(specularTexture, fTexCoords).rgb);
    if (on == 1)
    {
        color += (diffuseFlashlight * texture(diffuseTexture, fTexCoords).rgb + specularFlashlight * texture(specularTexture, fTexCoords).rgb);
    }

    fColor = vec4(color, 1.0f);
}