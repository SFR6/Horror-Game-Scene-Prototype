#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrix0;
uniform mat4 shadowMatrix1;
uniform mat4 shadowMatrix2;
uniform mat4 shadowMatrix3;
uniform mat4 shadowMatrix4;
uniform mat4 shadowMatrix5;

out vec4 FragPos;

mat4 shadowMatrices[6];

void main()
{
    shadowMatrices[0] = shadowMatrix0;
    shadowMatrices[1] = shadowMatrix1;
    shadowMatrices[2] = shadowMatrix2;
    shadowMatrices[3] = shadowMatrix3;
    shadowMatrices[4] = shadowMatrix4;
    shadowMatrices[5] = shadowMatrix5;

    int face, i;
    for (face = 0; face < 6; ++face)
    {
        if (face == 0)
        {
            gl_Layer = 0;
        }
        else if (face == 1)
        {
            gl_Layer = 1;
        }
        else if (face == 2)
        {
            gl_Layer = 2;
        }
        else if (face == 3)
        {
            gl_Layer = 3;
        }
        else if (face == 4)
        {
            gl_Layer = 4;
        }
        else
        {
            gl_Layer = 5;
        }

        //gl_Layer = face; // built-in variable that specifies to which face we render.
        for (i = 0; i < 3; ++i) // for each triangle vertex
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}