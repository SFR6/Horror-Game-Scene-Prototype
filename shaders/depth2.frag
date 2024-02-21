#version 410 core

in vec4 FragPos;

uniform vec3 lightPos;

out vec4 fColor;

void main() 
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far_plane
    float far_plane = 25.0f;
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}